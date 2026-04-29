#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseGun.generated.h"

UCLASS()
class SIMPLESHOOTER_API ABaseGun : public AActor
{
	GENERATED_BODY()

public:
	ABaseGun();

	virtual void Fire();
	virtual void Reload();

protected:
	// 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Gun|Component")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Gun|Component")
	UStaticMeshComponent* StaticMesh;

	// 파티클, 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gun|Effects")
	UParticleSystem* ParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gun|Effects")
	USoundBase* Sound;

	// 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gun|Ammo")
	int32 MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gun|Ammo")
	int32 RemainingAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gun|Damage")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gun|Interval")
	float FireInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gun|Interval")
	float LastFireTime;


private:
	bool CanFire() const;
};
 
