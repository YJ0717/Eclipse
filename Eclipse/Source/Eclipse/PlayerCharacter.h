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

	// UI에서 현재 스태미너를 가져올 수 있도록 BlueprintReadOnly로 설정합니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentStamina;

	// UI에서 최대 스태미너를 가져올 수 있도록 BlueprintReadOnly로 설정합니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float MaxStamina;

	// UI에서 현재 HP를 가져올 수 있도록 BlueprintReadOnly로 설정합니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth;

	// UI에서 최대 HP를 가져올 수 있도록 BlueprintReadOnly로 설정합니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float MaxHealth;

	// 피해를 입었을 때 호출될 함수 (오버라이드)
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

	// 플레이어 사망 처리 함수
	void Die();

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Attack();
	void Dodge(const FInputActionValue& Value);

	void StartWalking(const FInputActionValue& Value);
	void StopWalking(const FInputActionValue& Value);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnDeathAnimationEnded(UAnimMontage* Montage, bool bInterrupted); // 사망 애니메이션 종료 시 호출

	UFUNCTION()
	void OnHitAnimationEnded(UAnimMontage* Montage, bool bInterrupted); // 피격 애니메이션 종료 시 호출

    void ResetDodgeState();

	// AnimNotify에서 호출될 함수들
	UFUNCTION(BlueprintCallable)
	void SaveAttack_Notify();

	UFUNCTION(BlueprintCallable)
	void ResetCombo_Notify();

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

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

private:
	bool bIsRolling;

protected:
	// 스태미너 관련 변수들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float DodgeStaminaCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float StaminaRegenRate;

	bool bCanRegenStamina;

	// ... 나머지 코드 ...

		/** Called for forwards/backward input */
	void MoveForward(float Value);

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
	UAnimMontage* AttackMontage; // 이 변수는 더 이상 직접 사용되지 않지만, 다른 곳에서 쓸 수 있으니 남겨둡니다.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* HitMontage; // 피격 애니메이션 몽타주

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* DeathMontage; // 사망 애니메이션 몽타주

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
	FTimerHandle DodgeEndTimerHandle;
	FTimerHandle DeathTimerHandle; // 사망 애니메이션 타이머 핸들

	float OriginalMaxWalkSpeed;

	// -- 콤보 공격 시스템 --
	
	// 여러 공격 몽타주들을 순서대로 담을 배열
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TArray<UAnimMontage*> AttackMontages;

	// 현재 콤보 카운트
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack")
	int32 ComboCount;

	// 다음 공격이 요청되었는지 여부
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack")
	bool bNextAttackRequested;

	// 현재 공격 중인지 여부 (움직임 제한용)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack")
	bool bIsAttacking;
};