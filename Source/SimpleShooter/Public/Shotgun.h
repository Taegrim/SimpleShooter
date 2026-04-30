#pragma once

#include "CoreMinimal.h"
#include "BaseGun.h"
#include "Shotgun.generated.h"

UCLASS()
class SIMPLESHOOTER_API AShotgun : public ABaseGun
{
	GENERATED_BODY()

public:
    AShotgun();

protected:
    virtual void OnFire() override;

protected:
    // 흩어지는 탄알 개수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun|Pallet")
    int32 PelletCount;

    // 퍼지는 각도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun|Pallet")
    float SpreadAngle;
};
