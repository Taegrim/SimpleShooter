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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shotgun")
    int32 PelletCount;
};
