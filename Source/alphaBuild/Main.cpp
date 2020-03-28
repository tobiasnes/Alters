// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Engine/Public/TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"

// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Creates Boom that holds the camera so the player can always be seen
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 800.f;// Sets distance between Camera and player
	CameraBoom->RelativeRotation = FRotator(-80.f, 0.f, 0.f);
	CameraBoom->bUsePawnControlRotation = false; // Rotation not allowed

	// Set size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(45.f, 100.f);

	// creates the camera and attaches it to CameraBoom
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	PlayerCamera->bUsePawnControlRotation = false; // Rotation not allowed

	// Don't rotate when the controller rotates
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	StyleIndex = 1;
	MovementSpeedDash = 600.f;
	MovementSpeedFury = 300.f;
	MovementSpeedDefence = 150.f;
	MovementSpeedRanged = 250.f;

	DashDistance = 5000.f;
	DashCooldown = 0.5f;
	bCanDash = true;
	SpeedBeforeDash = FVector(0.f);
	DashStop = 0.1f;

	bEquipPressed = false;
	bWeaponEquipped = false;

	HP = 100;

}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character faces in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, TurnRate, 0.0f); // ...at ths rotation rate
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeedDash; // Sets Movement Speed
	
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveUp", this, &AMain::MoveUp);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);

	PlayerInputComponent->BindAction("DashStyle", IE_Pressed, this, &AMain::DashStyle);
	PlayerInputComponent->BindAction("FuryStyle", IE_Pressed, this, &AMain::FuryStyle);
	PlayerInputComponent->BindAction("DefenseStyle", IE_Pressed, this, &AMain::DefenseStyle);
	PlayerInputComponent->BindAction("RangedStyle", IE_Pressed, this, &AMain::RangedStyle);

	PlayerInputComponent->BindAction("Move1", IE_Pressed, this, &AMain::Move1Pressed);
	PlayerInputComponent->BindAction("Move1", IE_Released, this, &AMain::Move1Released);
	PlayerInputComponent->BindAction("Move2", IE_Pressed, this, &AMain::Move2Pressed);
	PlayerInputComponent->BindAction("Move2", IE_Released, this, &AMain::Move2Released);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AMain::EquipPressed);
	PlayerInputComponent->BindAction("Equip", IE_Released, this, &AMain::EquipReleased);
}	

void AMain::MoveUp(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && (!bAttacking))
	{
		// Finds the forward direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f); // FRotationmatrix want's a full FRotator so Rotation.Yaw won't work

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMain::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && (!bAttacking))
	{
		// Finds the forward direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f); // FRotationmatrix want's a full FRotator so Rotation.Yaw won't work

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AMain::EquipPressed()
{
	bEquipPressed = true;

	if (ActiveOverlappingItem)
	{
		
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			//EquipMesh();
			bFuryUnlocked = true;
			StyleIndex = 2;
			GetCharacterMovement()->MaxWalkSpeed = MovementSpeedFury;
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);
			bWeaponEquipped = true;
		}
	}
}
void AMain::EquipReleased()
{
	bEquipPressed = false;
}

void AMain::DashStyle()
{
	bAttacking = false;
	EquippedWeapon->DeactivateCollision();
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
		EquippedWeapon = false;
	}

	StyleIndex = 1;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeedDash;
	UE_LOG(LogTemp, Warning, TEXT("DashStyle()"));
	
}

void AMain::FuryStyle()
{
	bAttacking = false;
	EquippedWeapon->DeactivateCollision();
	if (bFuryUnlocked)
	{
		StyleIndex = 2;
		GetCharacterMovement()->MaxWalkSpeed = MovementSpeedFury;
		if(bWeaponEquipped == true)
		{

		GetWorld()->SpawnActor<AWeapon>(SpawnerClass, FTransform(GetActorLocation()));
		
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);
			UE_LOG(LogTemp, Warning, TEXT("you attacked"));
		}

		}
	}

	UE_LOG(LogTemp, Warning, TEXT("RageStyle()"));
}

void AMain::DefenseStyle()
{
	bAttacking = false;
	EquippedWeapon->DeactivateCollision();
		if (EquippedWeapon)
		{
			EquippedWeapon->Destroy();
			EquippedWeapon = false;
		}

	if (bDefenceUnlocked)
	{
		StyleIndex = 3;
		GetCharacterMovement()->MaxWalkSpeed = MovementSpeedDefence;
	}
	UE_LOG(LogTemp, Warning, TEXT("DefenseStyle()"));
}

void AMain::RangedStyle()
{
	bAttacking = false;
	EquippedWeapon->DeactivateCollision();
		if (EquippedWeapon)
		{
			EquippedWeapon->Destroy();
			EquippedWeapon = false;
		}

		if (bRangedUnlocked)
		{
			StyleIndex = 4;
			GetCharacterMovement()->MaxWalkSpeed = MovementSpeedRanged;
		}
		UE_LOG(LogTemp, Warning, TEXT("RangedStyle()"));
}

void AMain::Move1Pressed()
{
	bMove1Pressed = true;
	switch (StyleIndex)
	{
	case 1:
		UE_LOG(LogTemp, Warning, TEXT("Dash Move1 Pressed"));
		break;
	case 2:
		UE_LOG(LogTemp, Warning, TEXT("Fury Move1 Pressed"));
		
		Attack();

		break;
	case 3:
		UE_LOG(LogTemp, Warning, TEXT("Defense Move1 Pressed"));
		break;
	case 4:
		UE_LOG(LogTemp, Warning, TEXT("Ranged Move1 Pressed"));
		break;
	default:
		break;
	}
}

void AMain::Move1Released()
{
	bMove1Pressed = false;
	switch (StyleIndex)
	{
	case 1:
		UE_LOG(LogTemp, Warning, TEXT("Dash Move1 Released"));
		break;
	case 2:
		UE_LOG(LogTemp, Warning, TEXT("Fury Move1 Released"));
		break;
	case 3:
		UE_LOG(LogTemp, Warning, TEXT("Defense Move1 Released"));
		break;
	case 4:
		UE_LOG(LogTemp, Warning, TEXT("Ranged Move1 Released"));
		break;
	default:
		break;
	}
}

void AMain::Move2Pressed()
{
	bMove2Pressed = true;
	switch (StyleIndex)
	{
	case 1:
		Dash();
		UE_LOG(LogTemp, Warning, TEXT("Dash Move2 Pressed"));
		break;
	case 2:
		UE_LOG(LogTemp, Warning, TEXT("Fury Move2 Pressed"));

		Attack2();

		break;
	case 3:
		UE_LOG(LogTemp, Warning, TEXT("Defense Move2 Pressed"));
		break;
	case 4:
		UE_LOG(LogTemp, Warning, TEXT("Ranged Move2 Pressed"));
		break;
	default:
		break;
	}
}

void AMain::Move2Released()
{
	bMove2Pressed = false;
	switch (StyleIndex)
	{
	case 1:
		UE_LOG(LogTemp, Warning, TEXT("Dash Move2 Released"));
		break;
	case 2:
		UE_LOG(LogTemp, Warning, TEXT("Fury Move2 Released"));
		break;
	case 3:
		UE_LOG(LogTemp, Warning, TEXT("Defense Move2 Released"));
		break;
	case 4:
		UE_LOG(LogTemp, Warning, TEXT("Ranged Move2 Released"));
		break;
	default:
		break;
	}
}

void AMain::Dash()
{
	if (bCanDash)
	{
		SpeedBeforeDash = FVector(GetActorForwardVector().X, GetActorForwardVector().Y, 0.f) * MovementSpeedDash;
		GetCharacterMovement()->BrakingFrictionFactor = 0.f; // Removes friction
		LaunchCharacter(FVector(GetActorForwardVector().X, GetActorForwardVector().Y, 0.f) * DashDistance, true, true);
		bCanDash = false; // Disables Dash
		GetWorldTimerManager().SetTimer(DashHandle, this, &AMain::StopDashing, DashStop, false); // Sets timer to stop dashing
	}
	UE_LOG(LogTemp, Warning, TEXT("Dash()"));
}

void AMain::StopDashing()
{
	GetCharacterMovement()->StopMovementImmediately();
	LaunchCharacter(SpeedBeforeDash, true, true);
	GetCharacterMovement()->BrakingFrictionFactor = 2.f; // Sets friction back to default
	GetWorldTimerManager().SetTimer(DashHandle, this, &AMain::ResetDash, DashCooldown, false); // Sets timer to Enable dash again
}

void AMain::ResetDash()
{
	bCanDash = true; // Enables Dash
}

void AMain::SetEquippedWeapon(AWeapon* WeaponToSet)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}

	EquippedWeapon = WeaponToSet;
}

void AMain::Attack()
{
	if (!bAttacking)
	{
		bAttacking = true;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			AnimInstance->Montage_Play(CombatMontage, 1.35f);
			AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
			UE_LOG(LogTemp, Warning, TEXT("YOURE HERE LOL"));
		}
	}
}

void AMain::Attack2()
{
	if (!bAttacking)
	{
		bAttacking = true;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			AnimInstance->Montage_Play(CombatMontage, 1.35f);
			AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
			UE_LOG(LogTemp, Warning, TEXT("YOURE HERE LOL"));
		}
	}
}

void AMain::AttackEnd()
{
	bAttacking = false;
	if (bMove1Pressed)
	{
		Attack();
	}
	if (bMove2Pressed)
	{
		Attack2();
	}
}

void AMain::TakeDMG(float DamageValue, float KnockBackForce, FVector Direction)
{
	HP -= DamageValue; // Deals Damage
	LaunchCharacter(Direction * KnockBackForce, true, true); // launches player to make the player feel the force from the hit
	if (HP <= 0.f)
	{
		Destroy(); // Destroy the player if he looses all HP
	}
}