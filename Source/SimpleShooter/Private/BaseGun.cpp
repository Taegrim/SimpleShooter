#include "BaseGun.h"

#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "DrawDebugHelpers.h"

ABaseGun::ABaseGun()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	SetRootComponent(Scene);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(Scene);

    LastFireTime = -1000.f;
    MuzzleFlash = nullptr;
    FireSound = nullptr;
    MuzzleSocketName = TEXT("Muzzle");
    ReloadAnim = nullptr;
    MuzzleFlashRotationOffset = FRotator(0.f, -90.f, 0.f);
}

void ABaseGun::Fire()
{
    if(!CanFire()) return;

    --RemainingAmmo;
    LastFireTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

    PlayMuzzleFlash();
    PlayFireSound();

    BroadcastRecoil();

    OnFire();
}

void ABaseGun::Reload()
{
    RemainingAmmo = MaxAmmo;

    PlayReloadSound();
}

void ABaseGun::PlayMuzzleFlash()
{
    if (MuzzleFlash && StaticMesh)
    {
        // 나이아가라 이펙트가 회전이 달라서 오프셋을 적용해야 함
        UNiagaraFunctionLibrary::SpawnSystemAttached(
            MuzzleFlash,
            StaticMesh,
            MuzzleSocketName,
            FVector::ZeroVector,
            MuzzleFlashRotationOffset,
            EAttachLocation::SnapToTarget,
            true
            );
    }
}

void ABaseGun::PlayFireSound()
{
    if (FireSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            FireSound,
            GetActorLocation(),
            GetActorRotation()
            );
    }
}

void ABaseGun::PlayReloadSound()
{
    if (ReloadSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            ReloadSound,
            GetActorLocation(),
            GetActorRotation()
            );
    }
}

void ABaseGun::BeginPlay()
{
    Super::BeginPlay();

    RemainingAmmo = MaxAmmo;
}

// 공용 사격 처리 함수, LineTrace 한번 호출함
void ABaseGun::OnFire()
{
    FireLineTrace(GetMuzzleLocation(), GetMuzzleForwardVector());
}

// 연결한 모든 액터에 반동을 처리하라고 알리는 함수
void ABaseGun::BroadcastRecoil()
{
    OnGunRecoil.Broadcast(RecoilData);
}

void ABaseGun::FireLineTrace(const FVector& Start, const FVector& Direction)
{
    UWorld* World = GetWorld();
    if (!World) return;

    constexpr float TraceDistance = 10000.f;

    FVector End = Start + Direction * TraceDistance;

    // 자기자신 충돌 무시, 복잡한 충돌 false
    FCollisionQueryParams Params;
    Params.bTraceComplex = false;
    Params.AddIgnoredActor(this);

    // 총기 발사한 사람도 충돌 무시
    if (AActor* GunOwner = GetOwner())
    {
        Params.AddIgnoredActor(GunOwner);
    }

    FHitResult HitResult;
    const bool bHit = World->LineTraceSingleByChannel(
        HitResult,
        Start,
        End,
        ECC_Visibility,
        Params
        );

    // 충돌했다면 충돌 지점까지가 디버그 라인 끝
    const FVector DebugEnd = bHit ? HitResult.ImpactPoint : End;
    DrawDebugLine(
        World,
        Start,
        DebugEnd,
        FColor::Green,
        false,
        1.0f,
        0,
        1.5f
    );

    // 충돌 처리
    if (bHit)
    {
        DrawDebugPoint(
            World,
            HitResult.ImpactPoint,
            12.f,
            FColor::Red,
            false,
            1.f
            );

        // 이후에 ApplyDamage 처리
    }
}

UAnimMontage* ABaseGun::GetReloadAnim() const
{
    return ReloadAnim;
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

FTransform ABaseGun::GetMuzzleTransform() const
{
    if (StaticMesh && StaticMesh->DoesSocketExist(MuzzleSocketName))
    {
        return StaticMesh->GetSocketTransform(MuzzleSocketName);
    }

    return GetActorTransform();
}

FVector ABaseGun::GetMuzzleLocation() const
{
    return GetMuzzleTransform().GetLocation();
}

FVector ABaseGun::GetMuzzleForwardVector() const
{
    return GetMuzzleTransform().GetRotation().GetForwardVector();
}
