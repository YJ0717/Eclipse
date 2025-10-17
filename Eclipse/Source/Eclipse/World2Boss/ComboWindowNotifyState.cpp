#include "World2Boss/ComboWindowNotifyState.h"
#include "World2Boss/World2AIBossCharacter.h"

void UComboWindowNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (AWorld2AIBossCharacter* Boss = Cast<AWorld2AIBossCharacter>(MeshComp->GetOwner()))
    {
        UE_LOG(LogTemp, Log, TEXT("ComboWindow BEGIN: Setting bCanDoNextCombo = true"));
        Boss->bCanDoNextCombo = true;
    }
}

void UComboWindowNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);

    if (AWorld2AIBossCharacter* Boss = Cast<AWorld2AIBossCharacter>(MeshComp->GetOwner()))
    {
        Boss->bCanDoNextCombo = false;
    }
}
