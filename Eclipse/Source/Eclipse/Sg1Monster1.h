// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Sg1Monster1.generated.h"

UENUM(BlueprintType)
enum class EMonsterState : uint8
{
	Patrol,
	Chase,
	Attack,
	Dead
};

UCLASS()
class ECLIPSE_API ASg1Monster1 : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASg1Monster1();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	class UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EMonsterState CurrentState = EMonsterState::Patrol;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float AttackRange = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float ChaseSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float PatrolSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Patrol")
	float PatrolRadius = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Patrol")
	float PatrolWaitTime = 2.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	FVector HomeLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	FVector PatrolTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class APlayerCharacter* TargetPawn;

	// 플레이어를 잊어버리는 데 걸리는 시간입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float ForgetTime = 3.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// 플레이어가 마지막으로 목격된 시간입니다.
	float LastSeenTime = 0.0f;
	UFUNCTION()
	void OnSeePawn(APawn* Pawn);

	UFUNCTION()
	void OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume);
	
	void SetState(EMonsterState NewState);

	void HandlePatrolState(float DeltaTime);
	void HandleChaseState(float DeltaTime);
	void HandleAttackState(float DeltaTime);

	void FindNewPatrolLocation();

	FTimerHandle PatrolTimerHandle;

	float CurrentPatrolWaitTime;
};