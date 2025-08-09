#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/PawnSensingComponent.h"
#include "Sg1Monster1.generated.h"

UENUM(BlueprintType)
enum class EMonsterState : uint8
{
	EMS_Patrolling UMETA(DisplayName = "Patrolling"),
	EMS_Chasing UMETA(DisplayName = "Chasing"),
	EMS_Attacking UMETA(DisplayName = "Attacking"),
	EMS_Stunned UMETA(DisplayName = "Stunned"),
	EMS_Dead UMETA(DisplayName = "Dead")
};

UCLASS()
class ECLIPSE_API ASg1Monster1 : public ACharacter
{
	GENERATED_BODY()

public:
	ASg1Monster1();
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	EMonsterState MonsterState;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float Health;

	UPROPERTY(EditAnywhere, Category = "AI")
	float PatrolRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	class UAnimMontage* AttackMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	class UAnimMontage* HitMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	class UAnimMontage* DeathMontage;

private:
	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	UFUNCTION()
	void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void UpdateAIState();
	void Patrol();
	void Chase();
	void Attack();
	void Die();
	void ResetState();
	void MoveToRandomLocation();

	UPROPERTY()
	class AAIController* AIController;

	float LastSeenTime;
	float ChaseTimeout;
	FTimerHandle AttackTimerHandle;
	FTimerHandle StunResetTimerHandle;
};