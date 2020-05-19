// Fill out your copyright notice in the Description page of Project Settings.


#include "Cat.h"
#include "Main.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Runtime/UMG/Public/UMG.h"


ACat::ACat()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	CatEncounterEnd = false;
}

void ACat::BeginPlay()
{
	Super::BeginPlay();

}

void ACat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Time += DeltaTime;
	if (CatEncounterEnd == true)
	{
		//MainCharacter->CatEncounterEnd = false;
		Destroy();
	}
	
	if (Target)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(Target->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, 1.f);

		SetActorRotation(InterpRotation);

		LookAtYaw = GetTargetLookAtRotationYaw(Target->GetActorLocation());
		InterpRotation = FMath::RInterpTo(Target->GetActorRotation(), LookAtYaw, DeltaTime, 1.f);

		Target->SetActorRotation(InterpRotation);
	}

}

FRotator ACat::GetLookAtRotationYaw(FVector TargetLocation)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);
	FRotator YawLookAtRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);
	return YawLookAtRotation;
}

FRotator ACat::GetTargetLookAtRotationYaw(FVector TargetLocation)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(TargetLocation, GetActorLocation());
	FRotator YawLookAtRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);
	return YawLookAtRotation;
}

void ACat::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			if (Main->CatEncounter1Over == false)
			{
				Main->CatEncounter1Start = true;
				//Main->DialogueInstance1->AddToViewport();
			}
			else 
			{
				Main->CatEncounter2Start = true;
			}
			Target = Main;
			SavedMaxWalkSpeed = Main->GetCharacterMovement()->MaxWalkSpeed;
			SavedRotationRate = Main->GetCharacterMovement()->RotationRate;
			Main->Freeze();
			Main->SetCameraDistance(400.f);
		}
	}
}

void ACat::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->UnFreeze(SavedMaxWalkSpeed, SavedRotationRate);
			Main->SetCameraDistance(1000.f);
		}
	}
}
