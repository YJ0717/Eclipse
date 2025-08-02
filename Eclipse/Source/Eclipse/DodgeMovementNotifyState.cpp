// Fill out your copyright notice in the Description page of Project Settings.

#include "DodgeMovementNotifyState.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDodgeMovementNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	PlayerCharacter = Cast<ACharacter>(MeshComp->GetOwner());
	if (PlayerCharacter)
	{
		MovementComponent = PlayerCharacter->GetCharacterMovement();
		if (MovementComponent)
		{
			OriginalMovementMode = MovementComponent->MovementMode;
			MovementComponent->SetMovementMode(EMovementMode::MOVE_Flying);
		}
	}
	ElapsedTime = 0.f;
}

void UDodgeMovementNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	if (PlayerCharacter && MovementComponent && MovementCurve)
	{
		ElapsedTime += FrameDeltaTime;
		const float Speed = MovementCurve->GetFloatValue(ElapsedTime);
		const FVector ForwardVector = PlayerCharacter->GetActorForwardVector();

		MovementComponent->Velocity = ForwardVector * Speed;
	}
}

void UDodgeMovementNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (MovementComponent)
	{
		MovementComponent->Velocity = FVector::ZeroVector;
		MovementComponent->SetMovementMode(OriginalMovementMode);
	}

	PlayerCharacter = nullptr;
	MovementComponent = nullptr;
}