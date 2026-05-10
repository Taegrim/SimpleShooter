// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Weapon/GunRecoilData.h"
#include "SimpleShooterCharacter.generated.h"

class UWidgetComponent;
class UHealthManager;
class ABaseGun;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ASimpleShooterCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

    /** 조준용 카메라 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    UCameraComponent* AimCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

    // 총기 발사 액션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* FireAction;

    // 재장전 액션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* ReloadAction;

    // 1번 슬롯으로 무기 교체 액션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* EquipWeaponSlot1Action;

    // 2번 슬롯으로 무기 교체 액션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* EquipWeaponSlot2Action;

    // 조준 액션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* AimAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gun", meta = (AllowPrivateAccess = "true"))
    TArray<TSubclassOf<ABaseGun>> GunClasses;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun", meta = (AllowPrivateAccess = "true"))
    FName WeaponSocketName = TEXT("WeaponSocket");

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun", meta = (AllowPrivateAccess = "true"))
    TArray<TObjectPtr<ABaseGun>> Guns;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ABaseGun> CurrentGun;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun", meta = (AllowPrivateAccess = "true"))
    bool bIsReloading;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun", meta = (AllowPrivateAccess = "true"))
    bool bIsAiming;

    // 체력 관리 컴포넌트
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UHealthManager> HealthComponent;

    // 사망 애니메이션 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UAnimMontage> DeathMontage;

    // 체력바 위젯 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UWidgetComponent> HPBar;

public:
	ASimpleShooterCharacter();

    // 재장전 애니메이션 몽타주에서 호출할 실제 재장전 이벤트 함수
    UFUNCTION(BlueprintCallable)
    void ApplyReload();

    // 사망 애니메이션 몽타주에서 호출할 랙돌 적용 함수
    UFUNCTION(BlueprintCallable)
    void EnableRagDoll();

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

    // 조준 액션 시작, 끝
    void Aiming(const FInputActionValue& Value);
    void StopAiming(const FInputActionValue& Value);

    void Fire();
    void Reload();
    void EquipWeaponSlot1();
    void EquipWeaponSlot2();
    void EquipGunByIndex(int32 Index);
    void SpawnGuns();
    void AttachGunToHand(ABaseGun* Gun);

    // 에임 카메라로 전환 함수, 토글
    void ChangeAimCamera(bool Value);

    // 카메라 반동값 추가 함수
    void AddCameraRecoil(const FGunRecoilData& RecoilData);

    // 카메라 반동 업데이트 함수
    void UpdateCameraRecoil();

    // 카메라 반동 정리 함수
    void ClearCameraRecoil();

    // 반동 적용 함수
    UFUNCTION()
    void OnRecoil(const FGunRecoilData& RecoilData);

    UFUNCTION()
    void OnReloadMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    // 체력 변화 시 호출할 델리게이트 이벤트 함수
    UFUNCTION()
    void HandleHealthChanged(float CurrentHealth, float MaxHealth, float ActualDamage);

    // 사망 시 호출할 델리게이트 이벤트 함수
    UFUNCTION()
    void HandlePlayerDeath(AController* InstigatorActor);

protected:
    // 카메라 반동 타이머
    FTimerHandle CameraRecoilTimerHandle;

    // 적용해야할 반동, 되돌려야 할 반동
    FVector2D PendingCameraRecoil;
    FVector2D AppliedCameraRecoil;

    // 반동 적용 간격
    float CameraRecoilTimerInterval;

    // 반동 적용 속도
    float CameraRecoilApplySpeed;

    // 반동 회복 속도
    float CameraRecoilRecoverySpeed;

protected:
    virtual void BeginPlay() override;

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

