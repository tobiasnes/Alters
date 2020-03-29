// Fill out your copyright notice in the Description page of Project Settings.


#include "BigLadAnimInstance.h"
#include "BigLad.h"


void UBigLadAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			BigLad = Cast<ABigLad>(Pawn);
		}
	}
}

void UBigLadAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{

		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			BigLad = Cast<ABigLad>(Pawn);
		}

	}
	
	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();
	}
}
