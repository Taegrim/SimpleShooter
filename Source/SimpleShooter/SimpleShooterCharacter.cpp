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
#include "Animation/AnimInstance.h"

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

    // 조준용 카메라
    AimCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("AimCamera"));
    AimCamera->SetupAttachment(RootComponent); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    AimCamera->bUsePawnControlRotation = true;


    bIsReloading = false;
    bIsAiming = false;
    CameraRecoilTimerInterval = 0.01f;
    CameraRecoilApplySpeed = 40.f;
    CameraRecoilRecoverySpeed = 10.f;
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ASimpleShooterCharacter::ApplyReload()
{
    if (CurrentGun && bIsReloading)
    {
        CurrentGun->Reload();
    }
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASimpleShooterCharacter::BeginPlay()
{
    Super::BeginPlay();

    ChangeAimCamera(false);

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
	    if (AimAction)
	    {
	        EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started,
	            this, &ASimpleShooterCharacter::Aiming);
	        EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed,
                this, &ASimpleShooterCharacter::StopAiming);
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

void ASimpleShooterCharacter::Aiming(const FInputActionValue& Value)
{
    bIsAiming = true;
    ChangeAimCamera(true);
}

void ASimpleShooterCharacter::StopAiming(const FInputActionValue& Value)
{
    bIsAiming = false;
    ChangeAimCamera(false);
}

void ASimpleShooterCharacter::Fire()
{
    if (bIsReloading) return;

    if (CurrentGun)
    {
        if (AimCamera && bIsAiming)
        {
            CurrentGun->FireInDirection(AimCamera->GetForwardVector());
        }
        else
        {
            CurrentGun->Fire();
        }
    }
}

void ASimpleShooterCharacter::Reload()
{
    if (bIsReloading) return;

    if (CurrentGun)
    {
        if (UAnimMontage* ReloadAnim = CurrentGun->GetReloadAnim())
        {
            bIsReloading = true;
            float Duration = PlayAnimMontage(ReloadAnim);

            // 애니메이션 재생 실패하면 빠져나옴
            if (Duration <= 0.f)
            {
                bIsReloading = false;
                return;
            }

            // 재장전 애니메이션 끝날 때 호출할 함수 바인딩
            UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
            if (!AnimInstance)
            {
                bIsReloading = false;
                return;
            }

            FOnMontageEnded EndDelegate;
            EndDelegate.BindUObject(this, &ASimpleShooterCharacter::OnReloadMontageEnded);

            AnimInstance->Montage_SetEndDelegate(EndDelegate, ReloadAnim);
        }
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
    if (bIsReloading) return;

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
    // 소유자, 행동 유발자를 나로 설정
    // 충돌이 있어도 항상 생성되게 함
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

                Gun->OnGunRecoil.AddDynamic(this, &ASimpleShooterCharacter::OnRecoil);

                Guns.Add(Gun);
            }
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

void ASimpleShooterCharacter::ChangeAimCamera(bool Value)
{
    if (Value)
    {
        FollowCamera->SetActive(false);
        AimCamera->SetActive(true);
    }
    else
    {
        FollowCamera->SetActive(true);
        AimCamera->SetActive(false);
    }
}

void ASimpleShooterCharacter::AddCameraRecoil(const FGunRecoilData& RecoilData)
{
    if (!bIsAiming) return;

    const float RandomYaw = FMath::RandRange(-RecoilData.Yaw,RecoilData.Yaw);

    PendingCameraRecoil.X -= RecoilData.Pitch * RecoilData.Strength / 3;
    PendingCameraRecoil.Y += RandomYaw * RecoilData.Strength / 3;
    CameraRecoilRecoverySpeed = RecoilData.RecoverySpeed;

    if (!GetWorldTimerManager().IsTimerActive(CameraRecoilTimerHandle))
    {
        GetWorldTimerManager().SetTimer(
            CameraRecoilTimerHandle,
            this,
            &ASimpleShooterCharacter::UpdateCameraRecoil,
            CameraRecoilTimerInterval,
            true
            );
    }
}

void ASimpleShooterCharacter::UpdateCameraRecoil()
{
    if (!Controller || !bIsAiming)
    {
        ClearCameraRecoil();
        return;
    }

    // 한번에 반동 적용할 수치 , 반동 적용 속도 * 간격으로 조정
    float ApplyStep = CameraRecoilApplySpeed * CameraRecoilTimerInterval;

    // Update 한번에 적용할 반동값 조정
    FVector2D RecoilToApply;
    RecoilToApply.X = FMath::Clamp(PendingCameraRecoil.X, -ApplyStep, ApplyStep);
    RecoilToApply.Y = FMath::Clamp(PendingCameraRecoil.Y, -ApplyStep, ApplyStep);

    // 카메라 흔들고, 반동 적용한 만큼 값 감소
    // 반동 적용이 남아있다면 return 으로 적용만 하도록 함
    if (!RecoilToApply.IsNearlyZero())
    {
        AddControllerPitchInput(-RecoilToApply.X);
        AddControllerYawInput(RecoilToApply.Y);

        PendingCameraRecoil -= RecoilToApply;
        AppliedCameraRecoil += RecoilToApply;
        return;
    }

    // 한번에 적용할 반동 회복값
    float RecoveryStep = CameraRecoilRecoverySpeed * CameraRecoilTimerInterval;

    // Update 한번에 적용할 회복값 조정
    FVector2D RecoilToRecover;
    RecoilToRecover.X = FMath::Clamp(AppliedCameraRecoil.X, -RecoveryStep, RecoveryStep);
    RecoilToRecover.Y = FMath::Clamp(AppliedCameraRecoil.Y, -RecoveryStep, RecoveryStep);

    // 적용 방향과 반대로 수치 조절
    // 반동 회복이 남아있다면 반동 회복만 하도록 함
    if (!RecoilToRecover.IsNearlyZero())
    {
        AddControllerPitchInput(RecoilToRecover.X);
        AddControllerYawInput(-RecoilToRecover.Y);

        AppliedCameraRecoil -= RecoilToRecover;
        return;
    }

    // 둘다 0에 근접하면 카메라 반동 타이머 제거
    ClearCameraRecoil();
}

void ASimpleShooterCharacter::ClearCameraRecoil()
{
    // 반동값 초기화
    PendingCameraRecoil = FVector2D::ZeroVector;
    AppliedCameraRecoil = FVector2D::ZeroVector;

    // 타이머 정리
    GetWorldTimerManager().ClearTimer(CameraRecoilTimerHandle);
}

// 사격 시 반동적용을 위해 호출되는 함수
void ASimpleShooterCharacter::OnRecoil(const FGunRecoilData& RecoilData)
{
    if (!GetMesh()) return;

    UAnimInstance* AnimInstace = GetMesh()->GetAnimInstance();
    if (!AnimInstace) return;

    UFunction* AddRecoilFunction = AnimInstace->FindFunction(TEXT("AddRecoil"));
    if (!AddRecoilFunction) return;

    struct FAddRecoilParams
    {
        FGunRecoilData RecoilData;
    };

    FAddRecoilParams Params;
    Params.RecoilData = RecoilData;

    AnimInstace->ProcessEvent(AddRecoilFunction, &Params);

    // 1인칭일땐 카메라까지 흔들기
    if (bIsAiming)
    {
        AddCameraRecoil(RecoilData);
    }
}

// 장전 애니메이션이 끝나고 호출되는 함수
void ASimpleShooterCharacter::OnReloadMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    bIsReloading = false;
}
