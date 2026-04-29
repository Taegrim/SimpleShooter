#include "BaseGun.h"

ABaseGun::ABaseGun()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	SetRootComponent(Scene);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(Scene);


}

void ABaseGun::Fire()
{
	UE_LOG(LogTemp, Warning, TEXT("Fire"));
}

void ABaseGun::Reload()
{
	UE_LOG(LogTemp, Warning, TEXT("Reload"));
}

bool ABaseGun::CanFire() const
{
	if (RemainingAmmo <= 0)
	{
		return false;
	}

	// 마지막 발사 시간 + 발사 간격이 현재 시간보다 작아야 발사 가능
	float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	if (LastFireTime + FireInterval > CurrentTime)
	{
		return false;
	}

	return true;
}
