// Fill out your copyright notice in the Description page of Project Settings.


#include "SmallEnemyAnimInstance.h"
#include "SmallEnemy.h"


void USmallEnemyAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			SmallEnemy = Cast<ASmallEnemy>(Pawn);
		}
	}
}

void USmallEnemyAnimInstance::UpdateAnimationProperties()
{
	
	
	if (Pawn == nullptr)
	{
		
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{	
			SmallEnemy = Cast<ASmallEnemy>(Pawn);
		}
		
	}
	
	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();
	}
}
