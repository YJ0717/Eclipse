
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Sg1BossCharacter.generated.h"

UCLASS()
class ECLIPSE_API ASg1BossCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ASg1BossCharacter();

protected:
    virtual void BeginPlay() override;

    // 공격 상태를 블루프린트에서 제어할 수 있도록 UPROPERTY를 추가합니다.
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sg1Boss|State")
    bool bIsAttacking = false;

public:
    virtual void Tick(float DeltaTime) override;

    // -- 기본 스탯 --
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sg1Boss|Stats")
    float MaxHealth = 2000.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sg1Boss|Stats")
    float CurrentHealth;

    // -- 공격 애니메이션 몽타주 --
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sg1Boss|Animation")
    UAnimMontage* LeftLegAttackMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sg1Boss|Animation")
    UAnimMontage* RightLegAttackMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sg1Boss|Animation")
    UAnimMontage* HeadAttackMontage;

private:
    // -- AI 및 공격 로직 --

    // 플레이어의 위치를 기반으로 어떤 공격을 할지 결정합니다.
    void DecideAttackPattern();

    // 실제 공격을 수행하는 함수들입니다.
    void PerformLeftLegAttack();
    void PerformRightLegAttack();
    void PerformHeadAttack();

    // 플레이어 캐릭터에 대한 참조를 저장합니다.
    UPROPERTY()
    ACharacter* PlayerCharacter;

    // 공격 쿨타임 관련 변수
    float AttackCooldown = 3.0f;
    float LastAttackTime = -999.0f;

    // 공격을 시작할 수 있는지 확인합니다.
    bool CanAttack() const;
};
