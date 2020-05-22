// Fill out your copyright notice in the Description page of Project Settings.


#include "Shield.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Main.h"
#include "Enemy.h"
#include "Pack.h"
#include "Projectile.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PlayerController.h"
#include "Particles/ParticleSystemComponent.h"

AShield::AShield()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetRootComponent());

	BashCollision = CreateDefaultSubobject<USphereComponent>(TEXT("BashCollision"));
	BashCollision->SetupAttachment(GetRootComponent());

	KnockBack = 4000.f;
	StunTime = 1.5f;
}

void AShield::BeginPlay()
{
	Super::BeginPlay();

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AShield::ShieldCombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AShield::ShieldCombatOnOverlapEnd);
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	BashCollision->OnComponentBeginOverlap.AddDynamic(this, &AShield::BashOnOverlapBegin);
	BashCollision->OnComponentEndOverlap.AddDynamic(this, &AShield::BashOnOverlapEnd);
	BashCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BashCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BashCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BashCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AShield::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if ((ShieldState == EShieldState::EWS_Pickup) && OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->SetActiveOverlappingItem(this);
		}
	}
}

void AShield::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->SetActiveOverlappingItem(nullptr);
		}
	}
}

void AShield::Equip(AMain* Char)
{
	if (Char)
	{
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		SkeletalMesh->SetSimulatePhysics(false);

		const USkeletalMeshSocket* LeftElbowSocket = Char->GetMesh()->GetSocketByName("Left_Elbow_jointSocket");
		EquippedOn = Cast<AMain>(Char);

		if (LeftElbowSocket)
		{
			LeftElbowSocket->AttachActor(this, Char->GetMesh());
			bRotate = false;
			Char->SetEquippedShield(this);
		}
		if (OnEquipSound)
		{
			UGameplayStatics::PlaySound2D(this, OnEquipSound);
		}
		if (!bWeaponParticles)
		{
			IdleParticlesComponent->Deactivate();
		}
	}
}

void AShield::ShieldCombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap Shield Begin"));
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			Enemy->bBlocked = true;
			if (Enemy->bBlocked)
			{
				UE_LOG(LogTemp, Warning, TEXT("Overlap Shield Begin"));
			}
		}
		AProjectile* Projectile = Cast<AProjectile>(OtherActor);
		if (Projectile)
		{
			Projectile->OverlapUtility();
		}
	}
}

void AShield::ShieldCombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap Shield End"));
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			Enemy->bBlocked = false;
		}
	}
}

void AShield::ActivateCollision()
{
	//Mesh->SetRelativeLocation(FVector(0.f, 10.f, 0.f));
	Mesh->SetWorldScale3D(FVector(2.f, 1.f, 2.f));
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	UE_LOG(LogTemp, Warning, TEXT("Activate Collision"));
}

void AShield::DeactivateCollision()
{
	//Mesh->SetRelativeLocation(FVector(0.f, 10.f, 6.f));
	Mesh->SetWorldScale3D(FVector(0.5f, 1.f, 0.5f));
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AShield::BashOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			FRotator ToEnemyRotation = UKismetMathLibrary::FindLookAtRotation(EquippedOn->GetActorLocation(), Enemy->GetActorLocation());
			FRotator YawToEnemyRotation = FRotator(0.f, ToEnemyRotation.Yaw, 0.f);
			// get forward vector
			FVector Direction = FRotationMatrix(YawToEnemyRotation).GetUnitAxis(EAxis::X);
			Direction += FVector(0.f, 0.f, 0.1f);
			Enemy->TakeDMG(0.f, KnockBack, Direction);
		}
		APack* Pack = Cast<APack>(OtherActor);
		if (Pack)
		{
			Pack->StunnStart(StunTime);
		}
	}
}

void AShield::BashOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AShield::ActivateBashCollision()
{
	if (ShieldBashParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShieldBashParticles, GetActorLocation(), FRotator(0.f), true);
	}
	Mesh->SetWorldScale3D(FVector(2.f, 1.f, 2.f));
	BashCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	UE_LOG(LogTemp, Warning, TEXT("Activate Collision"));
}

void AShield::DeactivateBashCollision()
{
	Mesh->SetWorldScale3D(FVector(0.5f, 1.f, 0.5f));
	BashCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
