
// Fill out your copyright notice in the Description page of Project Settings.

#include "Sg1BossCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h" // 이 줄이 추가되었습니다.

ASg1BossCharacter::ASg1BossCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // 보스가 움직이지 않도록 설정
    GetCharacterMovement()->SetMovementMode(MOVE_None);
}

void ASg1BossCharacter::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;

    // 게임이 시작될 때 플레이어 캐릭터를 찾아 저장해둡니다.
    PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    if (!PlayerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Sg1BossCharacter: PlayerCharacter not found!"));
    }
}

void ASg1BossCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 플레이어가 존재하고, 현재 공격 중이 아니며, 공격 쿨타임이 지났을 때만 AI 로직을 실행합니다.
    if (PlayerCharacter && CanAttack())
    {
        DecideAttackPattern();
    }
}

bool ASg1BossCharacter::CanAttack() const
{
    // 현재 공격 중이 아니고, 마지막 공격 시간으로부터 쿨타임만큼 시간이 지났는지 확인
    return !bIsAttacking && (GetWorld()->GetTimeSeconds() - LastAttackTime > AttackCooldown);
}

void ASg1BossCharacter::DecideAttackPattern()
{
    if (!PlayerCharacter) return;

    // 보스에서 플레이어로 향하는 방향 벡터를 구합니다.
    const FVector BossToPlayer = PlayerCharacter->GetActorLocation() - GetActorLocation();
    const FVector BossForwardVector = GetActorForwardVector();

    // 보스의 오른쪽 방향 벡터를 구합니다.
    const FVector BossRightVector = FVector::CrossProduct(FVector::UpVector, BossForwardVector);

    // 방향 벡터를 정규화합니다.
    const FVector BossToPlayerNormalized = BossToPlayer.GetSafeNormal();

    // 내적(Dot Product)을 사용하여 플레이어가 보스의 왼쪽에 있는지 오른쪽에 있는지 판단합니다.
    // 결과가 양수(+)이면 오른쪽, 음수(-)이면 왼쪽에 있다는 의미입니다.
    const float DotProductWithRight = FVector::DotProduct(BossToPlayerNormalized, BossRightVector);

    // 정면과의 각도를 계산하여 플레이어가 정면에 있는지 판단합니다.
    const float DotProductWithForward = FVector::DotProduct(BossToPlayerNormalized, BossForwardVector);
    const float AngleWithForward = FMath::Acos(DotProductWithForward);

    // 플레이어가 정면 30도 내에 있다면 헤드 어택
    if (FMath::RadiansToDegrees(AngleWithForward) < 30.0f)
    {
        PerformHeadAttack();
    }
    // 플레이어가 오른쪽에 있다면 라이트 레그 어택
    else if (DotProductWithRight > 0)
    {
        PerformRightLegAttack();
    }
    // 플레이어가 왼쪽에 있다면 레프트 레그 어택
    else
    {
        PerformLeftLegAttack();
    }
}

void ASg1BossCharacter::PerformLeftLegAttack()
{
    if (LeftLegAttackMontage)
    {
        bIsAttacking = true;
        LastAttackTime = GetWorld()->GetTimeSeconds();
        UE_LOG(LogTemp, Log, TEXT("Performing Left Leg Attack"));

        // 몽타주 재생
        PlayAnimMontage(LeftLegAttackMontage);

        // TODO: 몽타주가 끝나면 bIsAttacking을 false로 설정하는 로직 추가 필요 (AnimNotify 또는 타이머 사용)
    }
}

void ASg1BossCharacter::PerformRightLegAttack()
{
    if (RightLegAttackMontage)
    {
        bIsAttacking = true;
        LastAttackTime = GetWorld()->GetTimeSeconds();
        UE_LOG(LogTemp, Log, TEXT("Performing Right Leg Attack"));
        PlayAnimMontage(RightLegAttackMontage);
    }
}

void ASg1BossCharacter::PerformHeadAttack()
{
    if (HeadAttackMontage)
    {
        bIsAttacking = true;
        LastAttackTime = GetWorld()->GetTimeSeconds();
        UE_LOG(LogTemp, Log, TEXT("Performing Head Attack"));
        PlayAnimMontage(HeadAttackMontage);
    }
}


