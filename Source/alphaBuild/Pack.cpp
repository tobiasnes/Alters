// Fill out your copyright notice in the Description page of Project Settings.


#include "Pack.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "AIController.h"
#include "AIModule.h"
#include "Main.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"

APack::APack()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AggroSphere = CreateAbstractDefaultSubobject<USphereComponent>(TEXT("AggroSphere"));
	AggroSphere->SetupAttachment(GetRootComponent());
	AggroSphere->InitSphereRadius(600.f);

	CombatSphere = CreateAbstractDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.f);

	AttackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackBox"));
	AttackBox->SetupAttachment(GetRootComponent());

	bOverlappingCombatSphere = false;
	bOverlappingAggroSphere = false;
	MainInHitRange = false;

	InterpSpeed = 10.f;
	bInterpToMain = false;

	bStunned = false;

}

// Called when the game starts or when spawned
void APack::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());

	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &APack::AggroSphereOnOverlapBegin);
	AggroSphere->OnComponentEndOverlap.AddDynamic(this, &APack::AggroSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &APack::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &APack::CombatSphereOnOverlapEnd);

	AttackBox->OnComponentBeginOverlap.AddDynamic(this, &APack::AttackBoxOnOverlapBegin);
	AttackBox->OnComponentEndOverlap.AddDynamic(this, &APack::AttackBoxOnOverlapEnd);

}

// Called every frame
void APack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterpToMain && CombatTarget && !bStunned)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}

}

FRotator APack::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator YawLookAtRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);
	return YawLookAtRotation;
}

// Called to bind functionality to input
void APack::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APack::AggroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			MoveTo = Main;
			MoveToTarget(Main);
			bOverlappingAggroSphere = true;
		}
	}
}

void APack::AggroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				MoveTo = nullptr;
				if (!bStunned)
				{
					SetSmallEnemyMovementStatus(EPackMovementStatus::EMS_Idle);
				}
				if (AIController)
				{
					AIController->StopMovement();
					bOverlappingAggroSphere = false;
				}
			}
		}
	}
}

void APack::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				Main->SetCombatTarget(this);
				CombatTarget = Main;
				bOverlappingCombatSphere = true;
				SetInterpToMain(true);
				if (!bStunned)
				{
					SetSmallEnemyMovementStatus(EPackMovementStatus::EMS_Attacking);
				}
			}
		}
	}
}

void APack::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				Main->SetCombatTarget(nullptr);
				bOverlappingCombatSphere = false;
				SetInterpToMain(false);
				if (PackMovementStatus != EPackMovementStatus::EMS_Attacking)
				{
					MoveToTarget(Main);
					CombatTarget = nullptr;
				}
			}
		}
	}
}

void APack::MoveToTarget(class AMain* Target)
{
	if (!bStunned)
	{
		SetSmallEnemyMovementStatus(EPackMovementStatus::EMS_MoveToTarget);
	}

	if (AIController)
	{
		FAIMoveRequest MoveRequest; // Struct containing the target
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(25.0f); // minmum distance between EnemyHitBox and TargetHitBox before the enemy stops moving

		FNavPathSharedPtr NavPath; // Will store navigatin data for the Enemy AI to use

		AIController->MoveTo(MoveRequest, &NavPath); // Makes the Enemy follow player until it gets close enough to attack

		/**
		auto PathPoints = NavPath->GetPathPoints();
		for (auto Point : PathPoints)
		{
			FVector Location = Point.Location;

			UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Red, 10.f, 1.5f);
		}
		*/
	}
}

void APack::TakeDMG(float DamageValue, float KnockBackForce, FVector Direction)
{
	Super::TakeDMG(DamageValue, KnockBackForce, Direction);
}

void APack::AttackBoxOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				MainInHitRange = true;
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("AttackBoxOnOverlapBegin()"));
}

void APack::AttackBoxOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				MainInHitRange = false;
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("AttackBoxOnOverlapEnd()"));
}

void APack::HitPlayer()
{
	if (MainInHitRange)
	{
		// cast TakeDMG function on player
		if (CombatTarget && !bBlocked)
		{
			USceneComponent* SceneComp = Cast<USceneComponent>(GetComponentByClass(USceneComponent::StaticClass()));
			const FRotator Rotation = SceneComp->GetComponentRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			// get forward vector
			FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			float x = Direction.X;
			float y = Direction.Y;
			float z = Direction.Z;

			UE_LOG(LogTemp, Warning, TEXT("x: %f y: %f z: %f"), x, y, z);
			Cast<AMain>(CombatTarget)->TakeDMG(Damage, KnockBack, Direction);
		}
		UE_LOG(LogTemp, Warning, TEXT("Player Got Hit!"));
	}
	UE_LOG(LogTemp, Warning, TEXT("HitPlayer()"));
}

void APack::SetInterpToMain(bool interp)
{
	bInterpToMain = interp;
}

void APack::StunnStart(float Time)
{
	bStunned = true;
	PackMovementStatus = EPackMovementStatus::EMS_Stunned;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 0.0f;
	GetWorldTimerManager().SetTimer(StunHandle, this, &APack::StunnEnd, Time);
}

void APack::StunnEnd()
{
	bStunned = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, TurnRate, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	if (bOverlappingCombatSphere)
	{
		PackMovementStatus = EPackMovementStatus::EMS_Attacking;
	}
	else if (bOverlappingAggroSphere)
	{
		PackMovementStatus = EPackMovementStatus::EMS_MoveToTarget;
		MoveToTarget(MoveTo);
	}
	else
	{
		PackMovementStatus = EPackMovementStatus::EMS_Idle;
	}
	GetWorld()->GetTimerManager().ClearTimer(StunHandle);
}