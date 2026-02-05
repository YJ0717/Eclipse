// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotifyState_Sg4BossMelee.h"
#include "Sg4BossCharacter.h"

void UAnimNotifyState_Sg4BossMelee::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (ASg4BossCharacter* Boss = Cast<ASg4BossCharacter>(MeshComp->GetOwner()))
	{
		Boss->ActivateSwordCollision();
		UE_LOG(LogTemp, Warning, TEXT("Sg4BossMelee BEGIN: Sword collision ACTIVATED"));
	}
}

void UAnimNotifyState_Sg4BossMelee::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (ASg4BossCharacter* Boss = Cast<ASg4BossCharacter>(MeshComp->GetOwner()))
	{
		Boss->DeactivateSwordCollision();
		UE_LOG(LogTemp, Warning, TEXT("Sg4BossMelee END: Sword collision DEACTIVATED"));
	}
}
