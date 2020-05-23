// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"
#include "Main.h"
#include "Projectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ATurret::ATurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());

	AggroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AggroSphere"));
	AggroSphere->SetupAttachment(GetRootComponent());
	AggroSphere->InitSphereRadius(1000.f);

	bActive = true;
	FireDelay = 1.f;
	FireTime = 0.f;
	CombatTarget = nullptr;

}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();

	FireTime = FireDelay;
	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &ATurret::AggroSphereOnOverlapBegin);
	AggroSphere->OnComponentEndOverlap.AddDynamic(this, &ATurret::AggroSphereOnOverlapEnd);
	
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FireTime < FireDelay)
	{
		FireTime += DeltaTime;
	}

	if (bActive && CombatTarget && FireTime >= FireDelay)
	{
		FRotator ToTargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CombatTarget->GetActorLocation());
		FRotator YawToTargetRotation = FRotator(0.f, ToTargetRotation.Yaw, 0.f);
		// get forward vector
		FVector Direction = FRotationMatrix(YawToTargetRotation).GetUnitAxis(EAxis::X);

		FTransform CrystalSpawnTransform;
		CrystalSpawnTransform.SetLocation(GetActorLocation() + (Direction * 10.f) + FVector(0.f, 0.f, 50.f));
		CrystalSpawnTransform.SetRotation(YawToTargetRotation.Quaternion());
		CrystalSpawnTransform.SetScale3D(FVector(1.f));
		GetWorld()->SpawnActor<AProjectile>(CrystalSpawnerClass, CrystalSpawnTransform);
		FireTime = 0.f;
	}
}

void ATurret::AggroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			CombatTarget = Main;
		}
	}
}

void ATurret::AggroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			CombatTarget = nullptr;
		}
	}
}