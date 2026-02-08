// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Sg3BossCharacter.generated.h"

class APlayerCharacter;
class UBoxComponent;
class UAnimMontage;

UENUM(BlueprintType)
enum class ESg3BossState : uint8
{
	Watching            UMETA(DisplayName = "Watching"),
	Circling            UMETA(DisplayName = "Circling"),
	Approaching         UMETA(DisplayName = "Approaching"),
	BackingOff          UMETA(DisplayName = "BackingOff"),
	Attacking           UMETA(DisplayName = "Attacking"),
	AttackCooldown      UMETA(DisplayName = "Attack Cooldown"),
	ChargeAttacking     UMETA(DisplayName = "Charge Attacking"), // 돌진공격 추가!
	Dead                UMETA(DisplayName = "Dead")
};

UCLASS()
class ECLIPSE_API ASg3BossCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASg3BossCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 애니메이션에서 호출할 수 있도록 public!
	UFUNCTION(BlueprintCallable)
	void ActivateWeaponCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateWeaponCollision();

	UFUNCTION(BlueprintCallable)
	void StopChargeAttack(); // NotifyState가 끝날 때 호출!

protected:
	virtual void BeginPlay() override;

private:
	void FacePlayer(float DeltaTime);
	void MakeDecision();
	void ExecuteState(float DeltaTime);
	void PerformAttack();
	void PerformChargeAttack(); // 차지어택 함수 추가!
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void PerformDodge(); // 회피 함수 추가
	void OnDodgeEnd(); // 회피 종료 함수 추가

	UFUNCTION()
	void OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);



protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Stats")
	float MaxHealth = 2000.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Stats")
	float CurrentHealth;

	bool bIsDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float AttackCooldownDuration = 2.0f;

	// AI 상태 변수
	ESg3BossState CurrentAIState;
	FTimerHandle DecisionTimer;
	TArray<AActor*> HitActors;

	// 공격 상태 플래그
	bool bIsChargeAttacking; // 차지어택 진행 중

	// 회피 관련
	bool bIsDodging;
	FTimerHandle DodgeTimer;
	FVector LastPlayerAttackLocation;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	APlayerCharacter* PlayerCharacter;

	// --- AI 행동 파라미터 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
	float AttackRange = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
	float RepositionDistance = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
	float ChargeAttackDistance = 1000.0f; // 돌진공격 발동 거리 추가!

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
	float DecisionInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
	float RetreatDuration = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
	float ChargeDuration = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
	float ChargeAttackDuration = 2.0f; // 차지어택 지속 시간 추가!

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
	float DodgeDistance = 400.0f; // 회피 거리

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
	float DodgeDuration = 0.5f; // 회피 지속 시간

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
	float DodgeDetectionRange = 500.0f; // 플레이어 공격 감지 범위

	// --- AI 속도 파라미터 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float WalkSpeed = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float StrafeSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float ChargeSpeed = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float ChargeAttackSpeed = 1800.0f; // 돌진공격 속도 추가!

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float RotationSpeed = 12.0f;

	// --- AI 전투 파라미터 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float AttackDamage = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float ChargeAttackDamage = 40.0f; // 돌진공격 데미지 추가!

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat")
	TArray<UAnimMontage*> AttackMontages; // 공격1, 2

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat")
	UAnimMontage* ChargeAttackMontage; // 돌진공격 몽타주 추가!

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat")
	UAnimMontage* DeathMontage;

	UPROPERTY(VisibleDefaultsOnly, Category = "AI|Combat")
	UBoxComponent* RightWeaponCollisionBox;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void DieUI();

	bool bIsAttacking;
};