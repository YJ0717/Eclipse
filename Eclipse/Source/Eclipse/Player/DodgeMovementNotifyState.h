// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "DodgeMovementNotifyState.generated.h"

// Forward declarations to let the compiler know these types exist.
class UCurveFloat;
class ACharacter;
class UCharacterMovementComponent;

/**
 *
 */
UCLASS()
class ECLIPSE_API UDodgeMovementNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	// 구르기 속도를 제어하는 커브
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	UCurveFloat* MovementCurve;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
	// We use pointers, so forward declarations are enough here.
	ACharacter* PlayerCharacter;
	UCharacterMovementComponent* MovementComponent;
	TEnumAsByte<EMovementMode> OriginalMovementMode;
	float ElapsedTime;
};