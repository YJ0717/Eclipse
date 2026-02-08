// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Sg4BossCharacter.generated.h"

class APlayerCharacter;
class UBoxComponent;
class UAnimMontage;

UENUM(BlueprintType)
enum class ESg4BossState : uint8
{
	Watching            UMETA(DisplayName = "Watching"),
	Circling            UMETA(DisplayName = "Circling"),
	Approaching         UMETA(DisplayName = "Approaching"),
	BackingOff          UMETA(DisplayName = "BackingOff"),
	MeleeAttacking      UMETA(DisplayName = "Melee Attacking"),      // 검 공격
	RangedAttacking     UMETA(DisplayName = "Ranged Attacking"),    // 마법 공격
	AttackCooldown      UMETA(DisplayName = "Attack Cooldown"),
	Dead                UMETA(DisplayName = "Dead")
};

UCLASS()
class ECLIPSE_API ASg4BossCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASg4BossCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// 노티파이용 public 함수
	UFUNCTION(BlueprintCallable)
	void ActivateSwordCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateSwordCollision();

	UFUNCTION(BlueprintCallable)
	void FireMagicProjectile(); // 마법 발사!

protected:
	virtual void BeginPlay() override;

private:
	void FacePlayer(float DeltaTime);
	void MakeDecision();
	void ExecuteState(float DeltaTime);
	void PerformMeleeAttack();  // 근접 공격
	void PerformRangedAttack(); // 원거리 공격
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


protected:
	// ============================================
	// 스탯
	// ============================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Stats")
	float MaxHealth = 3000.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Stats")
	float CurrentHealth;

	bool bIsDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float AttackCooldownDuration = 2.5f;

	// AI 상태 변수
	ESg4BossState CurrentAIState;
	FTimerHandle DecisionTimer;
	TArray<AActor*> HitActors;

	// 선회 방향
	float CirclingDirection;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	APlayerCharacter* PlayerCharacter;

	// --- AI 행동 파라미터 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
	float MeleeAttackRange = 400.0f; // 검 공격 거리

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
	float RangedAttackRange = 1200.0f; // 마법 공격 최대 거리

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
	float RepositionDistance = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
	float DecisionInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
	float RetreatDuration = 1.5f;

	// --- AI 속도 파라미터 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float WalkSpeed = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float StrafeSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float ChargeSpeed = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float RotationSpeed = 12.0f;

	// --- AI 전투 파라미터 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float MeleeDamage = 30.0f; // 검 데미지

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float MagicDamage = 40.0f; // 마법 데미지

	// 검 공격 몽타주 (배열)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat")
	TArray<UAnimMontage*> MeleeAttackMontages;

	// 마법 공격 몽타주 (배열)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat")
	TArray<UAnimMontage*> RangedAttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat")
	UAnimMontage* DeathMontage;

	// 마법 투사체 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat")
	TSubclassOf<AActor> MagicProjectileClass;

	// 검 충돌 박스 (R_Weapon001)
	UPROPERTY(VisibleDefaultsOnly, Category = "AI|Combat")
	UBoxComponent* SwordCollisionBox;

	// 마법 발사 소켓 (L_Weapon001)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	FName MagicSocketName = TEXT("L_Weapon001");

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void DieUI();

	bool bIsAttacking;
};