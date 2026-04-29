// Copyright Epic Games, Inc. All Rights Reserved.

#include "SimpleShooterCharacter.h"

#include "BaseGun.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ASimpleShooterCharacter

ASimpleShooterCharacter::ASimpleShooterCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASimpleShooterCharacter::BeginPlay()
{
    Super::BeginPlay();

    SpawnGuns();
    EquipWeaponSlot1();
}

void ASimpleShooterCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ASimpleShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASimpleShooterCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASimpleShooterCharacter::Look);

	    if (FireAction)
	    {
	        EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered,
	            this, &ASimpleShooterCharacter::Fire);
	    }
	    if (ReloadAction)
	    {
	        EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started,
                            this, &ASimpleShooterCharacter::Reload);
	    }
	    if (EquipWeaponSlot1Action)
	    {
	        EnhancedInputComponent->BindAction(EquipWeaponSlot1Action, ETriggerEvent::Started,
                this, &ASimpleShooterCharacter::EquipWeaponSlot1);
	    }
	    if (EquipWeaponSlot2Action)
	    {
	        EnhancedInputComponent->BindAction(EquipWeaponSlot2Action, ETriggerEvent::Started,
                this, &ASimpleShooterCharacter::EquipWeaponSlot2);
	    }
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASimpleShooterCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ASimpleShooterCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ASimpleShooterCharacter::Fire()
{
    if (CurrentGun)
    {
        CurrentGun->Fire();
    }
}

void ASimpleShooterCharacter::Reload()
{
    if (CurrentGun)
    {
        CurrentGun->Reload();
    }
}

void ASimpleShooterCharacter::EquipWeaponSlot1()
{
    EquipGunByIndex(0);
}

void ASimpleShooterCharacter::EquipWeaponSlot2()
{
    EquipGunByIndex(1);
}

void ASimpleShooterCharacter::EquipGunByIndex(int32 Index)
{
    // 인덱스 유효성 검사, 총기 유효성 검사
    if (!Guns.IsValidIndex(Index) || !Guns[Index]) return;

    // 바꾸려는 총이 현재 총이라면 빠져나옴
    if (CurrentGun == Guns[Index]) return;

    // 현재 총기 비활성화
    if (CurrentGun)
    {
        CurrentGun->SetActorHiddenInGame(true);
        CurrentGun->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    }

    CurrentGun = Guns[Index];

    CurrentGun->SetActorHiddenInGame(false);
    AttachGunToHand(CurrentGun.Get());
}

void ASimpleShooterCharacter::SpawnGuns()
{
    Guns.Reset();

    // 스폰 파라미터 설정
    // 소유자, 행동 유발자를 나로 설정, 충돌이 있어도 항상 생성되게 함
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    for (const TSubclassOf<ABaseGun>& GunClass : GunClasses)
    {
        if (GunClass)
        {
            ABaseGun* Gun = GetWorld() ?
                GetWorld()->SpawnActor<ABaseGun>(GunClass, GetActorTransform(), SpawnParams) :
                nullptr;

            if (Gun)
            {
                Gun->SetActorHiddenInGame(true);
                Gun->SetActorEnableCollision(false);
            }

            Guns.Add(Gun);
        }
    }
}

void ASimpleShooterCharacter::AttachGunToHand(ABaseGun* Gun)
{
    if (!Gun) return;

    Gun->AttachToComponent(
        GetMesh(),
        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        WeaponSocketName
        );
}
