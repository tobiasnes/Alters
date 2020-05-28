// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAnimInstance.h"
#include "Boss.h"


void UBossAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			Boss = Cast<ABoss>(Pawn);
		}
	}
}

void UBossAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{

		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			Boss = Cast<ABoss>(Pawn);
		}

	}

	if (Pawn)
	{
		if (Boss)
		{
			float X = Boss->MoveDirection.X;
			float Y = Boss->MoveDirection.Y;
			MovementSpeed = 100 * sqrt((X * X) + (Y * Y));
			UE_LOG(LogTemp, Warning, TEXT("%f %f %f"), X, Y, MovementSpeed);
			HP = Boss->HP;
		}
	}
}
