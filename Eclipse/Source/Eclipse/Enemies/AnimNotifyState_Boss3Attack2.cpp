// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotifyState_Boss3Attack2.h"
#include "Sg3BossCharacter.h"

void UAnimNotifyState_Boss3Attack2::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (ASg3BossCharacter* Boss = Cast<ASg3BossCharacter>(MeshComp->GetOwner()))
	{
		Boss->ActivateWeaponCollision();
		UE_LOG(LogTemp, Warning, TEXT("Boss3Attack2: Weapon collision ACTIVATED"));
	}
}

void UAnimNotifyState_Boss3Attack2::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (ASg3BossCharacter* Boss = Cast<ASg3BossCharacter>(MeshComp->GetOwner()))
	{
		Boss->DeactivateWeaponCollision();
		UE_LOG(LogTemp, Warning, TEXT("Boss3Attack2: Weapon collision DEACTIVATED"));
	}
}
