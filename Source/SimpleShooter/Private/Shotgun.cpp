#include "Shotgun.h"

AShotgun::AShotgun() : PelletCount(8), SpreadAngle(5.f)
{
}

void AShotgun::OnFireDirection(const FVector& Direction)
{
    FVector Start = GetMuzzleLocation();
    FVector Forward = Direction.GetSafeNormal();

    float SpreadAngleRadians = FMath::DegreesToRadians(SpreadAngle);

    // 팰릿 수만큼 Line Trace
    for (int32 i = 0; i < PelletCount; ++i)
    {
        // 특정 벡터 중심으로 원뿔 형태안의 무작위 방향벡터를 생성해주는 함수 VRandCone
        FVector PelletDirection = FMath::VRandCone(Forward, SpreadAngleRadians);
        FireLineTrace(Start, PelletDirection);
    }
}
