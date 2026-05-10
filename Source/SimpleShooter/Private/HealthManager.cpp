#include "HealthManager.h"

UHealthManager::UHealthManager()
{
	PrimaryComponentTick.bCanEverTick = false;

	MaxHealth = 100.f;
    bIsDead = false;
}


void UHealthManager::BeginPlay()
{
	Super::BeginPlay();

    CurrentHealth = MaxHealth;

    // 소유자의 TakeDamage에 이 컴포넌트의 TakeDamage 바인딩
    GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthManager::TakeDamage);
}

void UHealthManager::TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
    AController* Instigator, AActor* Causer)
{
    if (bIsDead) return;

    float ActualDamage = Damage;

    CurrentHealth -= ActualDamage;
    CurrentHealth = FMath::Max(CurrentHealth, 0.f);

    OnDamaged.Broadcast(CurrentHealth, MaxHealth, ActualDamage);
    if (CurrentHealth <= 0.f)
    {
        bIsDead = true;
        OnDeath.Broadcast(Instigator);
    }
}
