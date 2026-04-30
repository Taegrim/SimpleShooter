#pragma once

#include "CoreMinimal.h"
#include "GunRecoilData.generated.h"

USTRUCT(BlueprintType)
struct FGunRecoilData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
    float Strength;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
    float Pitch;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
    float Yaw;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
    float Roll;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
    float RecoverySpeed;
};

// RecoilData를 인자로 받는 이벤트 설정
// 이후에 UI, 카메라 등을 흔들 수 있기 때문에 Multicast 방식으로 이벤트 설정
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGunRecoil, const FGunRecoilData&, RecoilData);
