#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GunRecoilData.h"
#include "BaseGun.generated.h"

class UAnimMontage;
class UNiagaraSystem;

UCLASS()
class SIMPLESHOOTER_API ABaseGun : public AActor
{
	GENERATED_BODY()

public:
	ABaseGun();

	virtual void Fire();
	virtual void Reload();

    // 앞에서 설정한 이벤트를 멤버로 가짐
    UPROPERTY(BlueprintAssignable, Category = "Gun|Recoil")
    FOnGunRecoil OnGunRecoil;

    UAnimMontage* GetReloadAnim() const;

protected:
    virtual void BeginPlay() override;

    void PlayMuzzleFlash();
    void PlayFireSound();
    void PlayReloadSound();

    virtual void OnFire();

    // 연결한 액터들에게 반동 처리하라고 알리는 함수
    void BroadcastRecoil();

    // 라인 트레이스 수행
    void FireLineTrace(const FVector& Start, const FVector& Direction);

    // 총구 트랜스폼 관련
    FTransform GetMuzzleTransform() const;
    FVector GetMuzzleLocation() const;
    FVector GetMuzzleForwardVector() const;

protected:
	// 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun|Component")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun|Component")
	UStaticMeshComponent* StaticMesh;

	// 사격 나이아가라, 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun|Effects")
	UNiagaraSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun|Effects")
	USoundBase* FireSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun|Effects")
    USoundBase* ReloadSound;

	// 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun|Ammo")
	int32 MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun|Ammo")
	int32 RemainingAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun|Damage")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun|Interval")
	float FireInterval;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun|Interval")
	float LastFireTime;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun|Effects")
    FName MuzzleSocketName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun|Effects")
    FRotator MuzzleFlashRotationOffset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun|Recoil")
    FGunRecoilData RecoilData;

    // 총기 별 재장전 애니메이션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gun|Animation")
    TObjectPtr<UAnimMontage> ReloadAnim;

private:
	bool CanFire() const;
};

