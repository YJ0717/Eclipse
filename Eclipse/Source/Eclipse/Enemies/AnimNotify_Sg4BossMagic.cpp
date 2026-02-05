// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_Sg4BossMagic.h"
#include "Sg4BossCharacter.h"

void UAnimNotify_Sg4BossMagic::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (ASg4BossCharacter* Boss = Cast<ASg4BossCharacter>(MeshComp->GetOwner()))
	{
		Boss->FireMagicProjectile();
		UE_LOG(LogTemp, Warning, TEXT("Sg4BossMagic: Fire magic projectile!"));
	}
}

FString UAnimNotify_Sg4BossMagic::GetNotifyName_Implementation() const
{
	return FString("Sg4BossMagic");
}
