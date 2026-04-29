#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseGun.generated.h"


class UNiagaraSystem;

UCLASS()
class SIMPLESHOOTER_API ABaseGun : public AActor
{
	GENERATED_BODY()

public:
	ABaseGun();

	virtual void Fire();
	virtual void Reload();

protected:
    virtual void BeginPlay() override;

    virtual void PlayMuzzleFlash();
    virtual void PlayFireSound();
    virtual void PlayReloadSound();
    virtual void OnFire();

protected:
	// 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun|Component")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun|Component")
	UStaticMeshComponent* StaticMesh;

	// 나이아가라, 사운드
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun|Recoil")
    float RecoilStrength;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun|Recoil")
    float RecoilYawRange;

private:
	bool CanFire() const;
    void ApplyRecoil();
};

