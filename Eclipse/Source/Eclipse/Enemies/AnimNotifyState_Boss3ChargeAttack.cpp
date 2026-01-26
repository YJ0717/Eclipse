// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotifyState_Boss3ChargeAttack.h"
#include "Sg3BossCharacter.h"

void UAnimNotifyState_Boss3ChargeAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (ASg3BossCharacter* Boss = Cast<ASg3BossCharacter>(MeshComp->GetOwner()))
	{
		Boss->ActivateWeaponCollision();
		// bIsChargeAttacking은 PerformChargeAttack에서 이미 true로 설정됨
		UE_LOG(LogTemp, Warning, TEXT("Boss3ChargeAttack BEGIN: Weapon collision ACTIVATED, Duration=%.2f"), TotalDuration);
	}
}

void UAnimNotifyState_Boss3ChargeAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (ASg3BossCharacter* Boss = Cast<ASg3BossCharacter>(MeshComp->GetOwner()))
	{
		Boss->DeactivateWeaponCollision();
		Boss->StopChargeAttack(); // 돌진 즉시 중단!
		UE_LOG(LogTemp, Warning, TEXT("Boss3ChargeAttack END: Weapon collision DEACTIVATED, Charge STOPPED"));
	}
}
