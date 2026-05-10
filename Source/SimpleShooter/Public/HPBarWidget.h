#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HPBarWidget.generated.h"

class UProgressBar;

UCLASS()
class SIMPLESHOOTER_API UHPBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    void UpdateHPBar(float CurrentHealth, float MaxHealth);

protected:
    // UserWidget의 생성자, AddToViewport 시점에 호출됨
    virtual void NativeConstruct() override;

protected:
    // 위젯 블루프린트 안에있는 동일한 이름의 위젯을 자동으로 연결
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> HPBar;
};
