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

	// 노티파이에서 호출할 수 있도록 public으로 이동!
	UFUNCTION(BlueprintCallable)
	void ActivateWeaponCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateWeaponCollision();

protected:
	virtual void BeginPlay() override;

private:
	void FacePlayer(float DeltaTime);
	void MakeDecision();
	void ExecuteState(float DeltaTime);
	void PerformAttack();
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnChargeEnd();

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

	// 상태 제어 플래그
	bool bIsCharging;
	FTimerHandle ChargeTimer;
	float CirclingDirection;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	APlayerCharacter* PlayerCharacter;

	// --- AI 행동 파라미터 (World2Boss 완전 복사) ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
	float AttackRange = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
	float RepositionDistance = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
	float DecisionInterval = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
	float RetreatDuration = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
	float ChargeDuration = 0.7f;

	// --- AI 속도 파라미터 (World2Boss 완전 복사) ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float WalkSpeed = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float StrafeSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float ChargeSpeed = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float RotationSpeed = 5.0f;

	// --- AI 전투 파라미터 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float AttackDamage = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat")
	TArray<UAnimMontage*> AttackMontages; // 공격1, 2

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat")
	UAnimMontage* DeathMontage;

	UPROPERTY(VisibleDefaultsOnly, Category = "AI|Combat")
	UBoxComponent* RightWeaponCollisionBox;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void DieUI();

	bool bIsAttacking;
};
