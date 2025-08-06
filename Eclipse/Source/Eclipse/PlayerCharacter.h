// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h" 
#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;
class UBoxComponent; // <--- 추가

class ASg1Monster1;

UCLASS()
class ECLIPSE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Attack(); // <--- 변경
	void Dodge(const FInputActionValue& Value);

	void StartWalking(const FInputActionValue& Value);
	void StopWalking(const FInputActionValue& Value);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// AnimNotify에서 호출될 함수들
	UFUNCTION(BlueprintCallable)
	void StartAttackCollision();

	UFUNCTION(BlueprintCallable)
	void StopAttackCollision();

	// 충돌 이벤트 함수
	UFUNCTION()
	void OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DodgeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* WalkAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* AxeComponent;

	// 무기 충돌을 위한 박스 컴포넌트 <--- 추가
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	UBoxComponent* WeaponCollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bIsWeaponEquipped;

	UFUNCTION(BlueprintCallable, Category = Weapon)
	void ToggleWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* EquipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* UnequipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Effects, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactEffect;

	// 한 번의 공격 동안 이미 맞은 액터 목록 <--- 추가
	UPROPERTY()
	TArray<AActor*> HitActors;

	// 공격 데미지 <--- 추가
	UPROPERTY(EditAnywhere, Category = "Weapon")
	float AttackDamage = 25.0f;

	FVector PreviousBladeBaseLocation;
	FVector PreviousBladeTipLocation;

	float OriginalCapsuleHalfHeight;
	bool bIsDodgeEnding;
	float DodgeEndTimer;

	float OriginalMaxWalkSpeed;
};