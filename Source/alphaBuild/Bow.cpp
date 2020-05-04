// Fill out your copyright notice in the Description page of Project Settings.


#include "Bow.h"
#include "Components/SkeletalMeshComponent.h"
#include "Main.h"
#include "Enemy.h"
#include "Pack.h"
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
#include "Projectile.h"
#include "Engine/Public/TimerManager.h"


ABow::ABow()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	bCanShoot = false;

	SpawnTimer = 1.f;
}

void ABow::BeginPlay()
{
	Super::BeginPlay();

	bCanShoot = true;
}

void ABow::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if ((BowState == EBowState::EBS_Pickup) && OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->SetActiveOverlappingItem(this);
		}
	}
}

void ABow::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
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

void ABow::Equip(AMain* Char)
{
	if (Char)
	{
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		SkeletalMesh->SetSimulatePhysics(false);

		const USkeletalMeshSocket* LeftHandSocket = Char->GetMesh()->GetSocketByName("Left_Hand_jointSocket");
		EquippedOn = Cast<AMain>(Char);

		if (LeftHandSocket)
		{
			LeftHandSocket->AttachActor(this, Char->GetMesh());
			bRotate = false;
			Char->SetEquippedBow(this);
		}
		if (OnEquipSound)
		{
			UGameplayStatics::PlaySound2D(this, OnEquipSound);
		}
		if (!bBowParticles)
		{
			IdleParticlesComponent->Deactivate();
		}


	}
}

void ABow::SpawnArrow()
{
	if (ArrowSpawnerClass && bCanShoot)
	{

		FTransform ArrowSpawnTransform;
		FRotator Rotation = GetActorRotation();
		ArrowSpawnTransform.SetLocation(GetActorLocation() + FVector(-1.f, 1.f, 0.f));
		ArrowSpawnTransform.SetRotation(GetActorRotation().Quaternion());
		ArrowSpawnTransform.SetScale3D(FVector(1.f));

		GetWorld()->SpawnActor<AProjectile>(ArrowSpawnerClass, ArrowSpawnTransform);
		bCanShoot = false;
		GetWorldTimerManager().SetTimer(ShootHandle, this, &ABow::CanShootAgain, SpawnTimer, false);
	}
}

void ABow::CanShootAgain()
{
	bCanShoot = true;
}


/*void APlayerCharacter::Shoot()
{
	if (BulletClass)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParameters.bNoFail = true;
		SpawnParameters.Owner = this;
		SpawnParameters.Instigator = this;

		FTransform BulletSpawnTransform;
		BulletSpawnTransform.SetLocation(GetActorForwardVector() + GetActorLocation() + GetActorUpVector() * 40.f);
		BulletSpawnTransform.SetRotation(GetActorRotation().Quaternion());
		BulletSpawnTransform.SetScale3D(FVector(1.f));

		if (Time >= ShootDelay)
		{
			GetWorld()->SpawnActor<ABullet>(BulletClass, BulletSpawnTransform, SpawnParameters);
			Time = 0.f;

			if (ShootSound)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), ShootSound);
			}

		}

		UE_LOG(LogTemp, Warning, TEXT("Time is: %f"), Time);
	}

}*/