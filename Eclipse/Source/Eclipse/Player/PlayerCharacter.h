// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Components/TimelineComponent.h"
#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;
class UBoxComponent;
class UCurveFloat;
class UTimelineComponent;
class ASg1Monster2;
class ASg1Monster1;
class ASg1BossCharacter;



UENUM(BlueprintType)
enum class ETraitType : uint8
{
	AttackDamageUp     UMETA(DisplayName = "BaseAttackUp"),
	SkillBuff    UMETA(DisplayName = "SkillAttackUp"),
	SkillChange UMETA(DisplayName = "SkillChange")
};

USTRUCT(BlueprintType)
struct FTraitData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TraitName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TraitDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETraitType TraitType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;
};



UCLASS()
class ECLIPSE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	//로그라이크 특성용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traits")
	TArray<FTraitData> AllTraits;

	UFUNCTION(BlueprintCallable, Category = "Traits")
	void InitializeTraits();

	UFUNCTION(BlueprintCallable, Category = "Traits")
	TArray<FTraitData> GetRandomTraits(int32 Count);

	UFUNCTION(BlueprintCallable, Category = "Traits")
	void ApplyTrait(const FTraitData& Trait);


	//힐 관련 이미지용 함수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxEst;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float CurrentEst;
	//스킬용 함수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	TArray<UAnimMontage*> SkillMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float SkillAttack = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float SkillDamageRate = 1.0f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TArray<UAnimMontage*> RunAttackMontages;

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

	// 공격 데미지 <--- 추가
	UPROPERTY(EditAnywhere, Category = "Weapon")
	float AttackDamage = 25.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	float RiposteDamage = 150.0f;

	//카메라설정;;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MinTargetArmLength = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float DefaultTargetArmLength = 350.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BossNearDistance = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FVector DefaultSocketOffset = FVector(0.f, 60.f, 70.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FVector BossSocketOffset = FVector(0.f, 60.f, 200.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MinZ = 120.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MaxZ = 120.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MinX = -300.f; // 더 뒤로 밀고 싶으면 이 값 더 작게
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MaxX = -100.f; // 너무 멀리 가는 것 방지용

	//죽었을떄 UI용도
	UFUNCTION(BlueprintImplementableEvent, Category = "die")
	void PlayerDieUI();

	// 자유 시점 모드 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	bool bIsFreeLookMode;

	//카메라 보간용 함수
	void UpdateCameraByNearbyEnemies(float DeltaTime);
protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

protected:
	// 세이브/로드 함수
	void SavePlayerState();
	void LoadPlayerState();



	// 플레이어 사망 처리 함수
	void Die();


	// 타이틀 레벨로 돌아가는 함수
	void ReturnToTitle();

	// 이동할 타이틀 레벨의 이름
	UPROPERTY(EditDefaultsOnly, Category = "Level")
	FName TitleLevelName = "Start";

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Attack();
	void Dodge(const FInputActionValue& Value);
	void Parry(); // 패링 함수
	void ToggleGodMode();

	// 자유 시점 토글 함수
	void ToggleFreeLook();

	// Noclip 모드 수직 이동
	void MoveUp(const FInputActionValue& Value);
	void MoveDown(const FInputActionValue& Value);

	void Skill(const FInputActionValue& Value); // 스킬 함수
	void Heal(const FInputActionValue& Value); // HP회복
	void StartWalking(const FInputActionValue& Value);
	void StopWalking(const FInputActionValue& Value);
	bool bIsHealing = false;
	bool bIsRunningAttack = false;
	bool bIsUsingSkill = false;
	UFUNCTION()
	void OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnHealMontageEnded(UAnimMontage* Montage, bool bInterrupted);

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

	UFUNCTION(BlueprintCallable)
	void ApplyRiposteDamage();


	// 충돌 이벤트 함수
	UFUNCTION()
	void OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 무기가 DestructibleWall과 충돌했을 때 호출될 함수
	UFUNCTION()
	void OnWeaponHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ParryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* HealAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SaveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* GodModeAction;

	// 자유 시점 토글 입력 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FreeLookAction;

	// Noclip 모드 수직 이동 입력 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveDownAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
	float TurnRateGamepad;

private:
	bool bIsRolling;
	
	bool bIsGodMode;

	bool IsParryWindowActive() const;

	// 자유 시점 모드 이전 설정 저장
	bool bOriginalOrientRotationToMovement;

protected:
	// 스태미너 관련 변수들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float DodgeStaminaCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float StaminaRegenRate;

	bool bCanRegenStamina;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* ParryMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* RiposteMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Effects, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* HealMontage; // 힐용

	// 한 번의 공격 동안 이미 맞은 액터 목록 <--- 추가
	UPROPERTY()
	TArray<AActor*> HitActors;



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

	// -- 패링 시스템 --
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parry")
	float ParryStartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parry")
	float ParryEndTime;



	UPROPERTY()
	ASg1Monster1* RiposteTarget;

private:
	// -- 패링 카메라 줌 --
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UTimelineComponent* ParryCameraTimelineComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* ParryCameraCurve;

	UFUNCTION()
	void UpdateParryCamera(float Value);
};