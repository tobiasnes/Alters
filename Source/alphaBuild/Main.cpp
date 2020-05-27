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
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"
#include "Components/BoxComponent.h"
#include "Shield.h"
#include "Lever.h"
#include "Animation/AnimMontage.h"
#include "Bow.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Components/Decalcomponent.h"
#include "Materials/Material.h"
#include "UObject/ConstructorHelpers.h"


// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Creates Boom that holds the camera so the player can always be seen
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 1000.f;// Sets distance between Camera and player
	CameraBoom->RelativeRotation = FRotator(-80.f, 0.f, 0.f);
	CameraBoom->bUsePawnControlRotation = false; // Rotation not allowed
	bZoom = false;
	bReverseZoom = false;
	ZoomTarget = 0.f;

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

	//Sets DreamCatcher Mesh
	DreamCatcherMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DreamCatcherMesh"));
	DreamCatcherMesh->SetupAttachment(GetRootComponent());

	AimArrow = CreateDefaultSubobject<UDecalComponent>("AimArrow");
	AimArrow->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/Meshes/ArrowDecal/Arrow_Pointer_Mat.Arrow_Pointer_Mat'"));
	if (DecalMaterialAsset.Succeeded())
	{
		AimArrow->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	AimArrow->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	AimArrow->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

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
	bDashing = false;
	bIsInDashStyle = false;
	bIsInRangedStyle = false;

	bEquipPressed = false;
	bWeaponEquipped = false;

	bShieldEquipped = false;

	bBowEquipped = false;

	HP = 100;
	Frozen = false;

	bDashAttack = false;

	bFuryAttack1 = false;
	bFuryAttack2 = false;

	InterpSpeed = 15.f;
	bInterpToEnemy = false;
	
	bBlocking = false;

	bIsAiming = false;

	
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();


	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character faces in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, TurnRate, 0.0f); // ...at ths rotation rate
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeedDash; // Sets Movement Speed
	LastSafeDrop = GetActorLocation();
	bIsInDashStyle = true;
	AimArrow->SetHiddenInGame(true);
	SetActiveOverlappingItem(nullptr);
	
	UWorld* World = GetWorld();
	if (World)
	{
		FString CurrentLevel = World->GetMapName();
		FName CurrentLevelName(*CurrentLevel);
		//UEDPIE_0_Level_2
		if (CurrentLevelName == "UEDPIE_0_Level_2" || CurrentLevelName == "UEDPIE_0_Level_3" || CurrentLevelName == "Level_2" || CurrentLevelName == "Level_3")
		{
			bFuryUnlocked = true;
			bRangedUnlocked = true;
			bDefenceUnlocked = true;
			bDashKnifeUnlocked = true;
			bWeaponEquipped = true;
			bBowEquipped = true;
			bShieldEquipped = true;

			UE_LOG(LogTemp, Warning, TEXT("Equipped Lvl2 Gear."));
			UE_LOG(LogTemp, Warning, TEXT("%s"), *CurrentLevel);

			//GetWorld()->SpawnActor<AWeapon>(SpawnerClass, FTransform(GetActorLocation()));
			AWeapon* Weapon = Cast<AWeapon>(GetWorld()->SpawnActor<AWeapon>(SpawnerClass, FTransform(GetActorLocation())));
			if (Weapon)
			{
				UE_LOG(LogTemp, Warning, TEXT("Overlapping with Sword."));
				Weapon->Equip(this);
				SetActiveOverlappingItem(nullptr);
				if (EquippedWeapon)
				{
					EquippedWeapon->DeactivateCollision();
					//EquippedWeapon->Destroy();
					//EquippedWeapon = false;

					EquippedWeapon->KnifeMesh();
					EquippedWeapon->CombatCollision->SetRelativeScale3D(FVector(0.3f, 0.25f, 0.6f));
					EquippedWeapon->CombatCollision->SetRelativeLocation(FVector(0.5f, 0.f, 30.f));
				}
			}
		}
	}
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetActorLocation().Z < -400.f)
	{
		SetActorLocation(LastSafeDrop);
		TakeDMG(10.f, 0.f, FVector(0.f));
		GetCharacterMovement()->StopMovementImmediately();
	}

	if (bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}

	if (bZoom)
	{
		CameraBoom->TargetArmLength -= 500.f * DeltaTime;
		if (CameraBoom->TargetArmLength <= ZoomTarget)
		{
			bZoom = false;
		}
	}
	if (bReverseZoom)
	{
		CameraBoom->TargetArmLength += 500.f * DeltaTime;
		if (CameraBoom->TargetArmLength >= ZoomTarget)
		{
			bReverseZoom = false;
		}
	}

	if (bIsAiming)
	{
		FHitResult Hit;
		bool HitResult = false;
		

		HitResult = GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_WorldStatic), true, Hit);

		if(HitResult)
		{
			AimArrow->SetHiddenInGame(false);
			//GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
			FVector CursorFV = Hit.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();


			///Set the new direction of the pawn:
			FVector CursorLocation = Hit.Location;
			///Set Z to a little above ground
			FVector TempLocation = FVector(CursorLocation.X, CursorLocation.Y, 30.f);

			///Pure vector math
			FVector NewDirection = TempLocation - GetActorLocation();
			NewDirection.Z = 0.f;
			NewDirection.Normalize();
			SetActorRotation(NewDirection.Rotation());
		}
	}
}

FRotator AMain::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
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
	if ((Controller != nullptr) && (Value != 0.0f))
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
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// Finds the forward direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f); // FRotationmatrix want's a full FRotator so Rotation.Yaw won't work

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AMain::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

void AMain::EquipPressed()
{
	bEquipPressed = true;

	if (ActiveOverlappingItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		AShield* Shield = Cast<AShield>(ActiveOverlappingItem);
		ABow* Bow = Cast<ABow>(ActiveOverlappingItem);
		ALever* Lever = Cast<ALever>(ActiveOverlappingItem);
		if (Weapon)
		{
			bIsInDashStyle = false;
			if (EquippedShield)
			{
			EquippedShield->Destroy();
			EquippedShield = false;
			}
			else if (EquippedBow)
			{
				bIsAiming = false;
				EquippedBow->Destroy();
				EquippedBow = false;
				bIsInRangedStyle = false;
				AimArrow->SetHiddenInGame(true);
			}
			//EquipMesh();
			bFuryUnlocked = true;
			bDashKnifeUnlocked = true;
			bFuryWidget = true;
			StyleIndex = 2;
			GetCharacterMovement()->MaxWalkSpeed = MovementSpeedFury;
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);
			bWeaponEquipped = true;
		}
		else if (Shield)
		{
			bIsInDashStyle = false;
			if (EquippedWeapon)
			{
			EquippedWeapon->Destroy();
		    EquippedWeapon = false;
			}
			else if (EquippedBow)
			{
				bIsAiming = false;
				EquippedBow->Destroy();
				EquippedBow = false;
				bIsInRangedStyle = false;
				AimArrow->SetHiddenInGame(true);
			}
			bDefenceUnlocked = true;
			bDefenceWidget = true;
			StyleIndex = 3;
			GetCharacterMovement()->MaxWalkSpeed = MovementSpeedDefence;
			Shield->Equip(this);
			SetActiveOverlappingItem(nullptr);
			bShieldEquipped = true;
		}
		else if (Bow)
		{
			bIsInDashStyle = false;
			if (EquippedWeapon)
			{
				EquippedWeapon->Destroy();
				EquippedWeapon = false;
			}
			else if (EquippedShield)
			{
				EquippedShield->Destroy();
				EquippedShield = false;
			}
			bIsInRangedStyle = true;
			bRangedUnlocked = true;
			bRangedWidget = true;
			StyleIndex = 4;
			GetCharacterMovement()->MaxWalkSpeed = MovementSpeedRanged;
			Bow->Equip(this);
			SetActiveOverlappingItem(nullptr);
			bBowEquipped = true;
		}
		else if (Lever)
		{
			Lever->PullLever();
		}
	}
}
void AMain::EquipReleased()
{
	bEquipPressed = false;
}

void AMain::DashStyle()
{
	if ((StyleIndex != 1) && (!Frozen))
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->DeactivateCollision();
			//EquippedWeapon->Destroy();
			//EquippedWeapon = false;

			EquippedWeapon->KnifeMesh();
			EquippedWeapon->CombatCollision->SetRelativeScale3D(FVector(0.3f, 0.25f, 0.6f));
			EquippedWeapon->CombatCollision->SetRelativeLocation(FVector(0.5f, 0.f, 30.f));
		}
		else if (EquippedShield)
		{
			EquippedShield->DeactivateCollision();
			EquippedShield->CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			EquippedShield->Destroy();
			EquippedShield = false;
			UE_LOG(LogTemp, Warning, TEXT("DESTROYS SHIELD"))
		}
		else if (EquippedBow)
		{
			bIsAiming = false;
			EquippedBow->Destroy();
			EquippedBow = false;
			AimArrow->SetHiddenInGame(true);
			UE_LOG(LogTemp, Warning, TEXT("DESTROYS BOW"))
		}
		if (bDashKnifeUnlocked)
		{
			if ((bWeaponEquipped == true) && (StyleIndex != 2))
			{

				GetWorld()->SpawnActor<AWeapon>(SpawnerClass, FTransform(GetActorLocation()));

				AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
				if (Weapon)
				{
					Weapon->Equip(this);
					SetActiveOverlappingItem(nullptr);
					if (EquippedWeapon)
					{
						EquippedWeapon->DeactivateCollision();
						//EquippedWeapon->Destroy();
						//EquippedWeapon = false;

						EquippedWeapon->KnifeMesh();
						EquippedWeapon->CombatCollision->SetRelativeScale3D(FVector(0.3f, 0.25f, 0.6f));
						EquippedWeapon->CombatCollision->SetRelativeLocation(FVector(0.5f, 0.f, 30.f));
					}
				}

			}

		}
		bIsInDashStyle = true;
		bIsInRangedStyle = false;
		StyleIndex = 1;
		GetCharacterMovement()->MaxWalkSpeed = MovementSpeedDash;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("DashStyle()"));
}

void AMain::FuryStyle()
{
	if (bFuryUnlocked && (StyleIndex != 2) && (!Frozen))
	{
		bAttacking = false;
		if (EquippedWeapon)
		{
			EquippedWeapon->DeactivateCollision();
			EquippedWeapon->SwordMesh();
			EquippedWeapon->CombatCollision->SetRelativeScale3D(FVector(0.35f, 0.25f, 1.6f));
			EquippedWeapon->CombatCollision->SetRelativeLocation(FVector(0.5f, 0.f, 60.f));
		}
		else if (EquippedShield)
		{
			EquippedShield->DeactivateCollision();
			EquippedShield->CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			EquippedShield->Destroy();
			EquippedShield = false;
		}
		else if (EquippedBow)
		{
			bIsAiming = false;
			EquippedBow->Destroy();
			EquippedBow = false;
			AimArrow->SetHiddenInGame(true);
			UE_LOG(LogTemp, Warning, TEXT("DESTROYS BOW"))
		}
		if((bWeaponEquipped == true) && (StyleIndex != 1))
		{

		GetWorld()->SpawnActor<AWeapon>(SpawnerClass, FTransform(GetActorLocation()));
		
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);
			UE_LOG(LogTemp, Warning, TEXT("you attacked"));
			if (EquippedWeapon)
			{
				EquippedWeapon->DeactivateCollision();
				EquippedWeapon->SwordMesh();
				EquippedWeapon->CombatCollision->SetRelativeScale3D(FVector(0.35f, 0.25f, 1.6f));
				EquippedWeapon->CombatCollision->SetRelativeLocation(FVector(0.5f, 0.f, 60.f));
			}
		}

		}
		bIsInDashStyle = false;
		bIsInRangedStyle = false;
		StyleIndex = 2;
		GetCharacterMovement()->MaxWalkSpeed = MovementSpeedFury;
	}

	UE_LOG(LogTemp, Warning, TEXT("RageStyle()"));
}

void AMain::DefenseStyle()
{
	if (bDefenceUnlocked && (StyleIndex != 3) && (!Frozen))
	{
		bAttacking = false;
		if (EquippedWeapon)
		{
			EquippedWeapon->DeactivateCollision();
			EquippedWeapon->Destroy();
			EquippedWeapon = false;
		}
		else if (EquippedBow)
		{
			bIsAiming = false;
			EquippedBow->Destroy();
			EquippedBow = false;
			AimArrow->SetHiddenInGame(true);
			UE_LOG(LogTemp, Warning, TEXT("DESTROYS BOW"))
		}
		if ((bShieldEquipped == true) && (StyleIndex != 3))
		{

			GetWorld()->SpawnActor<AShield>(ShieldSpawnerClass, FTransform(GetActorLocation()));

			AShield* Shield = Cast<AShield>(ActiveOverlappingItem);

			if (Shield)
			{
				Shield->Equip(this);
				EquippedShield->DeactivateCollision();
				SetActiveOverlappingItem(nullptr);
			}

		}
		bIsInDashStyle = false;
		bIsInRangedStyle = false;
		StyleIndex = 3;
		GetCharacterMovement()->MaxWalkSpeed = MovementSpeedDefence;
	}
	UE_LOG(LogTemp, Warning, TEXT("DefenseStyle()"));
}

void AMain::RangedStyle()
{
	if (bRangedUnlocked && (StyleIndex != 4) && (!Frozen))
	{
		bAttacking = false;
		if (EquippedShield)
		{
			EquippedShield->DeactivateCollision();
			EquippedShield->CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			EquippedShield->Destroy();
			EquippedShield = false;
		}
		else if (EquippedWeapon)
		{
			EquippedWeapon->DeactivateCollision();
			EquippedWeapon->Destroy();
			EquippedWeapon = false;
		}
		if ((bBowEquipped == true) && (StyleIndex != 4))
		{

			GetWorld()->SpawnActor<ABow>(BowSpawnerClass, FTransform(GetActorLocation()));

			ABow* Bow = Cast<ABow>(ActiveOverlappingItem);

			if (Bow)
			{
				Bow->Equip(this);
				SetActiveOverlappingItem(nullptr);
			}

		}
		bIsInDashStyle = false;
		bIsInRangedStyle = true;
		StyleIndex = 4;
		GetCharacterMovement()->MaxWalkSpeed = MovementSpeedRanged;
	}
	UE_LOG(LogTemp, Warning, TEXT("RangedStyle()"));
}

void AMain::Move1Pressed()
{
	bMove1Pressed = true;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!Frozen)
	{
		switch (StyleIndex)
		{
		case 1:
			UE_LOG(LogTemp, Warning, TEXT("Dash Move1 Pressed"));
			if (bDashing && bDashKnifeUnlocked)
			{
				Attack4();
			}
			break;
		case 2:
			UE_LOG(LogTemp, Warning, TEXT("Fury Move1 Pressed"));

			Attack();

			break;
		case 3:
			BlockStart();

			UE_LOG(LogTemp, Warning, TEXT("Defense Move1 Pressed"));
			break;
		case 4:
			if (EquippedBow && bIsAiming)
			{
				EquippedBow->SpawnArrow();
			}

			UE_LOG(LogTemp, Warning, TEXT("Ranged Move1 Pressed"));
			break;
		default:
			break;
		}
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

		BlockEnd();

		UE_LOG(LogTemp, Warning, TEXT("We Got Here"))

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
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	if (!Frozen)
	{
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

			ShieldStun();

			break;
		case 4:
			if (EquippedBow && AnimInstance && AlterMontage)
			{
				bIsAiming = true;
				AnimInstance->Montage_Play(AlterMontage, 1.35f);
				AnimInstance->Montage_JumpToSection(FName("Bow_Draw"), AlterMontage);
			}

			UE_LOG(LogTemp, Warning, TEXT("Ranged Move2 Pressed"));
			break;
		default:
			break;
		}
	}
}

void AMain::Move2Released()
{
	bMove2Pressed = false;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
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

		if (EquippedBow && AnimInstance && AlterMontage)
		{
			AnimInstance->Montage_JumpToSection(FName("Bow_Release"), AlterMontage);
			bIsAiming = false;
			AimArrow->SetHiddenInGame(true);
		}

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
		bDashing = true;
		SpeedBeforeDash = FVector(GetActorForwardVector().X, GetActorForwardVector().Y, 0.f) * MovementSpeedDash;
		GetCharacterMovement()->BrakingFrictionFactor = 0.f; // Removes friction
		LaunchCharacter(FVector(GetActorForwardVector().X, GetActorForwardVector().Y, 0.1f) * DashDistance, true, true);
		bCanDash = false; // Disables Dash
		GetWorldTimerManager().SetTimer(DashHandle, this, &AMain::StopDashing, DashStop, false); // Sets timer to stop dashing
	}
	UE_LOG(LogTemp, Warning, TEXT("Dash()"));
}

void AMain::StopDashing()
{
	bDashing = false;
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

void AMain::SetEquippedShield(AShield* ShieldToSet)
{
	if (EquippedShield)
	{
		EquippedShield->Destroy();
	}

	EquippedShield = ShieldToSet;
}


void AMain::SetEquippedBow(ABow* BowToSet)
{
	if (EquippedBow)
	{
		EquippedBow->Destroy();
	}

	EquippedBow = BowToSet;
}

void AMain::Attack()
{
	if (!bAttacking)
	{
		bFuryAttack1 = true;
		bAttacking = true;
		SetInterpToEnemy(true);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && AlterMontage)
		{
			AnimInstance->Montage_Play(AlterMontage, 1.35f);
			AnimInstance->Montage_JumpToSection(FName("Attack_1"), AlterMontage);
			UE_LOG(LogTemp, Warning, TEXT("YOURE HERE LOL"));
		}
	}
}

void AMain::Attack2()
{
	if (!bAttacking)
	{
		bFuryAttack2 = true;
		bAttacking = true;
		SetInterpToEnemy(true);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && AlterMontage)
		{
			AnimInstance->Montage_Play(AlterMontage, 1.35f);
			AnimInstance->Montage_JumpToSection(FName("Attack_2"), AlterMontage);
			UE_LOG(LogTemp, Warning, TEXT("YOURE HERE LOL"));
		}
	}
}

void AMain::Attack4()
{
	if (!bAttacking)
	{
		bDashAttack = true;
		bAttacking = true;
		SetInterpToEnemy(true);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && AlterMontage)
		{
			AnimInstance->Montage_Play(AlterMontage, 1.35f);
			AnimInstance->Montage_JumpToSection(FName("Attack_4"), AlterMontage);
			UE_LOG(LogTemp, Warning, TEXT("YOURE HERE LOL"));
		}
	}
}

void AMain::AttackEnd()
{
	bAttacking = false;
	SetInterpToEnemy(false);
	bDashAttack = false;
	bFuryAttack1 = false;
	bFuryAttack2 = false;
	if (bMove1Pressed)
	{
		if (StyleIndex == 2)
		{
			Attack();
		}
		
	}
	if (bMove2Pressed)
	{
		if (StyleIndex == 2)
		{
			Attack2();
		}
	}
}

void AMain::BlockStart()
{
	bBlocking = true;
	/* do
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && AlterMontage)
		{
			AnimInstance->Montage_Play(AlterMontage, 1.35f);
			AnimInstance->Montage_JumpToSection(FName("Block_1"), AlterMontage);
			UE_LOG(LogTemp, Warning, TEXT("YOURE HERE LOL"));
		}
	} while (bBlocking == true); */
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (bBlocking)
	{
		
		if (AnimInstance && AlterMontage)
		{
			AnimInstance->Montage_Play(AlterMontage, 1.35f);
			AnimInstance->Montage_JumpToSection(FName("Block_1"), AlterMontage);
			UE_LOG(LogTemp, Warning, TEXT("YOURE HERE LOL"));
			if (EquippedShield)
			{
				EquippedShield->ActivateCollision();
			}
		}
	}
}

void AMain::BlockEnd()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	bBlocking = false;
	AnimInstance->Montage_Stop(0.3f, AlterMontage);
	if (EquippedShield)
	{
		EquippedShield->DeactivateCollision();
	}
}

void AMain::ShieldStun()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	if (!bBlocking)
	{
		if (AnimInstance && AlterMontage)
		{
			AnimInstance->Montage_Play(AlterMontage, 1.35f);
			AnimInstance->Montage_JumpToSection(FName("ShieldStun"), AlterMontage);
			UE_LOG(LogTemp, Warning, TEXT("YOURE HERE LOL"));
		}
	}
}

void AMain::TakeDMG(float DamageValue, float KnockBackForce, FVector Direction)
{
	HP -= DamageValue; // Deals Damage
	LaunchCharacter(Direction * KnockBackForce, true, true); // launches player to make the player feel the force from the hit
	if (HP <= 0.f)
	{
		Freeze();
	}
}

void AMain::Freeze()
{
	GetCharacterMovement()->MaxWalkSpeed = 0.f;
	GetCharacterMovement()->RotationRate = FRotator(0.f);
	Frozen = true;
	//UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	//if (AnimInstance && AlterMontage)
	//{
	//	AnimInstance->Montage_Play(AlterMontage, 1.35f);
	//	AnimInstance->Montage_JumpToSection(FName("Death"), AlterMontage);
	//}
	//UGameplayStatics::OpenLevel(this, TEXT("Game_Over"), false);
}

void AMain::UnFreeze(float SavedMaxWalkSpeed, FRotator SavedRotationRate)
{
	GetCharacterMovement()->MaxWalkSpeed = SavedMaxWalkSpeed;
	GetCharacterMovement()->RotationRate = SavedRotationRate;
	Frozen = false;
}

void AMain::SetCameraDistance(float Distance)
{
	ZoomTarget = Distance;
	if (ZoomTarget > CameraBoom->TargetArmLength)
	{
		bReverseZoom = true;
	}
	else if (ZoomTarget < CameraBoom->TargetArmLength)
	{
		bZoom = true;
	}
}

/*void AMain::SaveGame()
{
	UE_LOG(LogTemp, Warning, TEXT("Game Saved."));
	UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
	SaveGameInstance->PlayerLocation = this->GetActorLocation();
	SaveGameInstance->PlayerRotation = this->GetActorRotation();
	SaveGameInstance->PlayerHP = HP;

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("MySlot"), 0);
}

void AMain::LoadGame()
{
	UE_LOG(LogTemp, Warning, TEXT("Game Loaded."));
	UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));

	SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(("MySlot"), 0));

	this->SetActorLocation(SaveGameInstance->PlayerLocation);
	this->SetActorRotation(SaveGameInstance->PlayerRotation);
	HP = SaveGameInstance->PlayerHP;

}*/