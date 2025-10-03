
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Sg1BossCharacter.generated.h"


class APlayerCharacter;

// 어떤 공격 부위인지 구분하기 위한 Enum 입니다. 블루프린트에서도 쓸 수 있게 UENUM으로 선언해줍니다.
UENUM(BlueprintType)
enum class EAttackPart : uint8
{
	LeftLeg,
	RightLeg,
	Head,
	None
};

UCLASS()
class ECLIPSE_API ASg1BossCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	

	ASg1BossCharacter();
	//피
	UFUNCTION(BlueprintImplementableEvent, Category = "Boss|Effects")
	void OnPartDestroyed_BP(EAttackPart DestroyedPart);

	// 공격용 충돌 박스들을 선언합니다. VisibleAnywhere로 해두면 BP에서도 보고 편집할 수 있죠.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sg1Boss|Attack")
	class UBoxComponent* LeftLegAttackCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sg1Boss|Attack")
	class UBoxComponent* RightLegAttackCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sg1Boss|Attack")
	class UBoxComponent* HeadAttackCollision;
	//데미지
	// 피격용 충돌 박스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sg1Boss|Hit")
	class UBoxComponent* LeftLegHitCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sg1Boss|Hit")
	class UBoxComponent* RightLegHitCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sg1Boss|Hit")
	class UBoxComponent* HeadHitCollision;

	UPROPERTY()
	TArray<class UBoxComponent*> HitCollisions;

	EAttackPart DetectHitPart(FName BoneName);

protected:
	virtual void BeginPlay() override;

	// 공격 상태를 블루프린트에서 제어할 수 있도록 UPROPERTY를 추가합니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sg1Boss|State")
	bool bIsAttacking = false;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sg1Boss|Stats")
	TMap<EAttackPart, float> PartHealth;

	UPROPERTY(BlueprintReadOnly, Category = "Sg1Boss|Stats")
	float TotalCurrentHealth;

	

	// 지금 맞은 부위 저장
	EAttackPart LastHitPart;

	// -- 기본 스탯 --
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sg1Boss|Stats")
	float MaxHealth = 2000.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sg1Boss|Stats")
	float CurrentHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sg1Boss|Stats")
	float AttackDamage = 50.f;

	// -- 애니메이션 몽타주 --
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sg1Boss|Animation")
	UAnimMontage* LeftLegAttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sg1Boss|Animation")
	UAnimMontage* RightLegAttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sg1Boss|Animation")
	UAnimMontage* HeadAttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sg1Boss|Animation")
	UAnimMontage* DeathMontage;

	// -- 데미지 처리 --
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

	// -- 공격 판정 관련 함수 --
	UFUNCTION(BlueprintCallable, Category = "Sg1Boss|Attack")
	void ActivateAttackCollision(EAttackPart PartToActivate);

	UFUNCTION(BlueprintCallable, Category = "Sg1Boss|Attack")
	void DeactivateAttackCollision();

	// AnimNotify에서 호출하여 공격이 끝났음을 알리고 상태를 초기화하는 데 사용합니다.
	UFUNCTION(BlueprintCallable, Category = "Sg1Boss|Attack")
	void ResetAttackState();

private:
	// -- AI 및 공격 로직 --

	// 플레이어의 위치를 기반으로 어떤 공격을 할지 결정합니다.
	void DecideAttackPattern();

	// 실제 공격을 수행하는 함수들입니다.
	void PerformLeftLegAttack();
	void PerformRightLegAttack();
	void PerformHeadAttack();

	// 사망 처리
	void Die();

	// 공격이 플레이어와 겹쳤을 때 호출될 함수입니다.
	UFUNCTION()
	void OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	


	// 한 번의 공격에 여러 번 데미지가 들어가는 걸 방지하기 위해, 맞은 액터들을 잠시 저장해둘 배열입니다.
	TArray<AActor*> HitActors;

	// 플레이어 캐릭터에 대한 참조를 저장합니다.
	UPROPERTY()
	ACharacter* PlayerCharacter;

	// 공격 쿨타임 관련 변수
	float AttackCooldown = 3.0f;
	float LastAttackTime = -999.0f;

	// 공격을 시작할 수 있는지 확인합니다.
	bool CanAttack() const;
};

