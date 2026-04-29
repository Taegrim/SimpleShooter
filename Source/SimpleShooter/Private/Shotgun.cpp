#include "Shotgun.h"

AShotgun::AShotgun() : PelletCount(0)
{
    RecoilStrength = 2.0f;
    RecoilYawRange = 1.2f;
}

void AShotgun::OnFire()
{
    UE_LOG(LogTemp, Warning, TEXT("Shotgun Pellets Fire"));

    // 팰릿 수만큼 Line Trace
}
