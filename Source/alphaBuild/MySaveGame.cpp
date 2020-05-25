// Fill out your copyright notice in the Description page of Project Settings.


#include "MySaveGame.h"
#include "Main.h"

UMySaveGame::UMySaveGame()
{
	PlayerLocation = FVector(0.f, 0.f, 0.f);
	PlayerRotation = FRotator(0.f, 0.f, 0.f);

	PlayerHP = 0.f;

	/* MainCharacter->bFuryUnlocked = true;
	MainCharacter->bRangedUnlocked = true;
	MainCharacter->bDefenceUnlocked = true;
	MainCharacter->bWeaponEquipped = true;
	MainCharacter->bBowEquipped = true;
	MainCharacter->bShieldEquipped = true;*/
}

