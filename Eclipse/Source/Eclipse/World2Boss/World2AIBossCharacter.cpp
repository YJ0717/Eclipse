#include "World2AIBossCharacter.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

AWorld2AIBossCharacter::AWorld2AIBossCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = false;

    // 초기 상태 설정
    CurrentAIState = EBossAIState::Repositioning;
}

void AWorld2AIBossCharacter::BeginPlay()
{
    Super::BeginPlay();

    PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    // 게임 시작 시, 첫 의사결정을 내리도록 타이머를 설정합니다.
    GetWorldTimerManager().SetTimer(DecisionTimer, this, &AWorld2AIBossCharacter::MakeDecision, DecisionInterval, true, 0.5f);
}

void AWorld2AIBossCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (PlayerCharacter)
    {
        // 1. 플레이어를 항상 주시합니다.
        FacePlayer(DeltaTime);
        // 2. 현재 상태에 맞는 행동을 실행합니다.
        ExecuteState(DeltaTime);
    }
}

void AWorld2AIBossCharacter::FacePlayer(float DeltaTime)
{
    if (!PlayerCharacter) return;

    const FVector DirectionToPlayer = PlayerCharacter->GetActorLocation() - GetActorLocation();
    const FRotator TargetRotation = FRotator(0.0f, DirectionToPlayer.Rotation().Yaw, 0.0f);
    SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, RotationSpeed));
}

void AWorld2AIBossCharacter::MakeDecision()
{
    if (CurrentAIState == EBossAIState::Attacking || CurrentAIState == EBossAIState::Retreating) return;

    float Distance = GetDistanceTo(PlayerCharacter);

    // 플레이어가 공격 범위 밖에 있고, 현재 재배치 중이라면
    if (Distance > AttackRange && CurrentAIState == EBossAIState::Repositioning)
    {
        // 70% 확률로 공격을 위해 접근하기로 결정합니다.
        if (FMath::RandRange(0, 100) < 70)
        {
            CurrentAIState = EBossAIState::ClosingDistance;
        }
        // 나머지 30%는 계속 재배치(선회) 상태를 유지합니다.
    }
}

void AWorld2AIBossCharacter::ExecuteState(float DeltaTime)
{
    float Distance = GetDistanceTo(PlayerCharacter);

    switch (CurrentAIState)
    {
        case EBossAIState::Repositioning:
            GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
            // 재배치 거리(RepositionDistance)를 기준으로 거리를 조절하며 선회합니다.
            {
                FVector DirectionToTarget = (PlayerCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
                FVector StrafeDirection = FVector::CrossProduct(DirectionToTarget, FVector::UpVector) * CirclingDirection;
                FVector DistanceControl = (Distance > RepositionDistance) ? DirectionToTarget : -DirectionToTarget;
                AddMovementInput(StrafeDirection + DistanceControl * 0.5f);
            }
            break;

        case EBossAIState::ClosingDistance:
            GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
            // 플레이어를 향해 돌진합니다.
            AddMovementInput(GetActorForwardVector());
            // 공격 범위에 들어오면, 공격 상태로 전환합니다.
            if (Distance <= AttackRange)
            {
                CurrentAIState = EBossAIState::Attacking;
                // (임시) 공격 몽타주가 없으므로, 1초 후 후퇴 상태로 전환합니다.
                FTimerHandle TempAttackTimer;
                GetWorldTimerManager().SetTimer(TempAttackTimer, [this](){
                    CurrentAIState = EBossAIState::Retreating;
                }, 1.0f, false);
            }
            break;

        case EBossAIState::Attacking:
            // 공격 중에는 움직임을 멈춥니다.
            GetCharacterMovement()->StopMovementImmediately();
            break;

        case EBossAIState::Retreating:
            GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
            // 플레이어에게서 멀어집니다.
            AddMovementInput(GetActorForwardVector() * -1.0f);
            // 일정 시간 후퇴했으면, 다시 재배치 상태로 돌아갑니다.
            {
                FTimerHandle TempRetreatTimer;
                GetWorldTimerManager().SetTimer(TempRetreatTimer, [this](){
                    CurrentAIState = EBossAIState::Repositioning;
                    // 다음 선회 방향을 무작위로 다시 결정합니다.
                    CirclingDirection = FMath::RandBool() ? 1.0f : -1.0f;
                }, RetreatDuration, false);
            }
            break;
    }
}

float AWorld2AIBossCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    if (ActualDamage > 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("HIT! World2AIBossCharacter '%s' took %.2f damage from %s."), *GetName(), ActualDamage, *DamageCauser->GetName());
    }
    return ActualDamage;
}