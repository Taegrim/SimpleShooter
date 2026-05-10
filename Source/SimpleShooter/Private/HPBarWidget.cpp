#include "HPBarWidget.h"

#include "Components/ProgressBar.h"

void UHPBarWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (HPBar)
    {
        HPBar->SetPercent(1.f);
    }
}

void UHPBarWidget::UpdateHPBar(float CurrentHealth, float MaxHealth)
{
    if (!HPBar) return;

    float percent = MaxHealth <= 0.f ? 0.f : CurrentHealth / MaxHealth;

    HPBar->SetPercent(percent);
}
