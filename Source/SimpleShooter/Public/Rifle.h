#pragma once

#include "CoreMinimal.h"
#include "BaseGun.h"
#include "Rifle.generated.h"

UCLASS()
class SIMPLESHOOTER_API ARifle : public ABaseGun
{
	GENERATED_BODY()

public:
    ARifle();

protected:
    virtual void OnFire() override;
};
