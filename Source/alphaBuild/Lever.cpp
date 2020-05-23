// Fill out your copyright notice in the Description page of Project Settings.


#include "Lever.h"
#include "Main.h"
#include "Projectile.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALever::ALever()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LeverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeverMesh"));
	LeverMesh->SetupAttachment(GetRootComponent());

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	PlatformMesh->SetupAttachment(GetRootComponent());

	StartPoint = FVector(0.f);
	EndPoint = FVector(0.f);
	InterpSpeed = 4.f;
	RotationSpeed = 120.f;
	DegreesRotated = 0.f;

	bLeverActive = true;
	bActivatesOnOverlap = false;

}

// Called when the game starts or when spawned
void ALever::BeginPlay()
{
	Super::BeginPlay();

	PlatformMesh->AddLocalOffset(StartPoint);

	EndPoint += GetActorLocation();
	StartPoint += GetActorLocation();

	Distance = (EndPoint - StartPoint).Size();

	bInterping = false;
	bLeverMoves = false;

	CurrentLocation = StartPoint;

}

// Called every frame
void ALever::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bLeverMoves)
	{
		DegreesRotated += RotationSpeed * DeltaTime;
		LeverMesh->AddLocalRotation(FRotator(RotationSpeed * DeltaTime, 0.f, 0.f));
		if (DegreesRotated >= 60)
		{
			bLeverMoves = false;
		}
	}
	if (bInterping)
	{
		FVector Interp = FMath::VInterpTo(CurrentLocation, EndPoint, DeltaTime, InterpSpeed);
		Interp -= CurrentLocation;
		PlatformMesh->AddLocalOffset(Interp);
		CurrentLocation += Interp;

		float DistanceTraveled = (CurrentLocation - StartPoint).Size();
		if (Distance - DistanceTraveled <= 0.1f)
		{
			bInterping = false;
		}
	}

}

void ALever::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			if (bActivatesOnOverlap)
			{
				PullLever();
			}
			Main->SetActiveOverlappingItem(this);
		}
		AProjectile* Arrow = Cast<AProjectile>(OtherActor);
		if (Arrow)
		{
			if (!Arrow->bHarmsMain && !bActivatesOnOverlap)
			{
				PullLever();
			}
		}
	}
}

void ALever::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
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

void ALever::PullLever()
{
	if (bLeverActive)
	{
		if (ArrowOverlapParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ArrowOverlapParticles, GetActorLocation(), FRotator(0.f), true);
		}
		bLeverActive = false;
		bLeverMoves = true;
		bInterping = true;
	}
}