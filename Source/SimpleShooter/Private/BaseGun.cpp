#include "BaseGun.h"

#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

ABaseGun::ABaseGun()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	SetRootComponent(Scene);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(Scene);

    LastFireTime = 0;
    MuzzleFlash = nullptr;
    FireSound = nullptr;
    ReloadSound = nullptr;
    MuzzleSocketName = TEXT("Muzzle");
}

void ABaseGun::Fire()
{
    if(!CanFire()) return;

    --RemainingAmmo;
    LastFireTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

    PlayMuzzleFlash();
    PlayFireSound();
    ApplyRecoil();

    OnFire();
}

void ABaseGun::Reload()
{
    RemainingAmmo = MaxAmmo;

    PlayReloadSound();

	UE_LOG(LogTemp, Warning, TEXT("Reload"));
}

void ABaseGun::PlayMuzzleFlash()
{
    if (MuzzleFlash && StaticMesh)
    {
        UNiagaraFunctionLibrary::SpawnSystemAttached(
            MuzzleFlash,
            StaticMesh,
            MuzzleSocketName,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
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

// 디버그용, 자식 클래스에서 실제 처리해야함
void ABaseGun::OnFire()
{
    UE_LOG(LogTemp, Warning, TEXT("Base OnFire"));
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

// 반동 적용하는 함수
void ABaseGun::ApplyRecoil()
{
    // Player Spawn할때 Instigator 설정하므로 가져올 수 있음
    APawn* OwningPawn = GetInstigator();
    if (!OwningPawn)
    {
        // Instigator 없다면 Owner 가져옴
        OwningPawn = Cast<APawn>(GetOwner());
    }

    if (!OwningPawn) return;

    const float PitchRecoil = -RecoilStrength;
    const float YawRecoil = FMath::RandRange(-RecoilYawRange, RecoilYawRange);

    OwningPawn->AddControllerPitchInput(PitchRecoil);
    OwningPawn->AddControllerYawInput(YawRecoil);
}
