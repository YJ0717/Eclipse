#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/PawnSensingComponent.h"
#include "Sg1Monster2.generated.h"

UENUM(BlueprintType)
enum class EMonster2State : uint8
{
	EMS_Patrolling UMETA(DisplayName = "Patrolling"),
	EMS_Chasing UMETA(DisplayName = "Chasing"),
	EMS_Sidestep UMETA(DisplayName = "Sidestep"),
	EMS_Attacking UMETA(DisplayName = "Attacking"),
	EMS_Stunned UMETA(DisplayName = "Stunned"),
	EMS_Parried UMETA(DisplayName = "Parried"),
	EMS_Dead UMETA(DisplayName = "Dead")
};

UCLASS()
class ECLIPSE_API ASg1Monster2 : public ACharacter
{
	GENERATED_BODY()

public:
	ASg1Monster2();
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// AnimNotify에서 호출될 함수
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void AttackHitNotify();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Attack2HitNotify();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool IsStaggered() const;

	void GetParried();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	EMonster2State MonsterState;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float Health;

	UPROPERTY(EditAnywhere, Category = "AI")
	float PatrolRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TArray<UAnimMontage*> AttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	class UAnimMontage* HitMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	class UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	class UAnimMontage* StaggerMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	class UAnimMontage* SidestepMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float MonsterAttackDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class UBoxComponent* RightHandCollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class UBoxComponent* LeftHandCollisionBox;

	UPROPERTY()
	TArray<AActor*> HitActors; // HitActors 선언 추가

	UFUNCTION(BlueprintImplementableEvent)
	void OnDamaged_BP();

	float LastSidestepTime = -100.f;
	float SidestepCooldown = 2.0f;


private:
	UFUNCTION()
	void OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void AttackEndNotify(); // AttackEndNotify 선언 추가

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
	void RecoverFromStagger();
	void Sidestep();


	UPROPERTY()
	class AAIController* AIController;

	float LastSeenTime;
	float ChaseTimeout;
	FTimerHandle AttackTimerHandle;
	FTimerHandle StunResetTimerHandle;
	FTimerHandle StaggerTimerHandle;

	
};