// Fill out your copyright notice in the Description page of Project Settings.


#include "PackAnimInstance.h"
#include "Pack.h"


void UPackAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			Pack = Cast<APack>(Pawn);
		}
	}
}

void UPackAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			Pack = Cast<APack>(Pawn);
		}
	}

	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();
		if (Pack)
		{
			HP = Pack->HP;
		}
	}
}
