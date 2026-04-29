#include "Rifle.h"

ARifle::ARifle()
{
    RecoilStrength = 0.5f;
    RecoilYawRange = 0.3f;
}

void ARifle::OnFire()
{
    UE_LOG(LogTemp, Warning, TEXT("Rifle Fire"));
}
