#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthManager.generated.h"

// 사망, 피격 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeathSignature, AController*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDamagedSignature, float, CurrentHealth, float, MaxHealth, float, ActualDamage);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIMPLESHOOTER_API UHealthManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthManager();

    // 사망 델리게이트 객체
    UPROPERTY(BlueprintAssignable)
    FDeathSignature OnDeath;

    // 피격 델리게이트 객체
    UPROPERTY(BlueprintAssignable)
    FDamagedSignature OnDamaged;

protected:
	virtual void BeginPlay() override;

    UFUNCTION()
    void TakeDamage(AActor* DamagedActor,
        float Damage,
        const UDamageType* DamageType,
        AController* Instigator,
        AActor* Causer
        );

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
    float MaxHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
    float CurrentHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
    bool bIsDead;
};
