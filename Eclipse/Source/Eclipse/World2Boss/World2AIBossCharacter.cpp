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
    CurrentAIState = EBossAIState::Watching;
}

void AWorld2AIBossCharacter::BeginPlay()
{
    Super::BeginPlay();

    PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    // 첫 의사결정을 0.5초 후에 시작하고, 그 이후는 MakeDecision 함수 내에서 타이머를 관리합니다.
    GetWorldTimerManager().SetTimer(DecisionTimer, this, &AWorld2AIBossCharacter::MakeDecision, 0.5f, false);
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
    // 공격 중이거나 후퇴 중일 때는 새로운 결정을 내리지 않습니다.
    if (CurrentAIState == EBossAIState::Attacking || CurrentAIState == EBossAIState::BackingOff) return;

    float Distance = GetDistanceTo(PlayerCharacter);

    // 1. 플레이어가 공격 범위 안에 있는가?
    if (Distance <= AttackRange)
    {
        // 60% 확률로 공격, 40% 확률로 거리를 벌립니다. (바로 공격하지 않고 심리전)
        if (FMath::RandRange(0, 100) < 60)
        {
            CurrentAIState = EBossAIState::Attacking;
        }
        else
        {
            CurrentAIState = EBossAIState::BackingOff;
        }
    }
    // 2. 플레이어가 공격 범위 밖에 있는가?
    else if (Distance > RepositionDistance)
    {
        // 너무 멀다면, 80% 확률로 접근하고 20% 확률로 지켜봅니다.
        if (FMath::RandRange(0, 100) < 80)
        {
            CurrentAIState = EBossAIState::Approaching;
        }
        else
        {
            CurrentAIState = EBossAIState::Watching;
        }
    }
    // 3. 심리전을 벌이는 "중간" 거리인가?
    else
    {
        // 60% 확률로 선회하고, 25% 확률로 지켜보고, 15% 확률로 접근합니다.
        int32 RandVal = FMath::RandRange(0, 100);
        if (RandVal < 60)
        {
            CurrentAIState = EBossAIState::Circling;
            // 선회 방향을 무작위로 결정합니다. (좌우 버벅임 방지)
            CirclingDirection = FMath::RandBool() ? 1.0f : -1.0f;
        }
        else if (RandVal < 85)
        {
            CurrentAIState = EBossAIState::Watching;
        }
        else
        {
            CurrentAIState = EBossAIState::Approaching;
        }
    }

    // 결정된 상태를 실행하고, 다음 결정을 위한 타이머를 다시 설정합니다.
    GetWorldTimerManager().SetTimer(DecisionTimer, this, &AWorld2AIBossCharacter::MakeDecision, DecisionInterval, false);
}

void AWorld2AIBossCharacter::ExecuteState(float DeltaTime)
{
    FVector MoveDirection = FVector::ZeroVector;

    switch (CurrentAIState)
    {
        case EBossAIState::Watching:
            GetCharacterMovement()->MaxWalkSpeed = 0; // 제자리에서 관망
            break;

        case EBossAIState::Circling:
            GetCharacterMovement()->MaxWalkSpeed = StrafeSpeed; // 선회 속도 적용
            MoveDirection = GetActorRightVector() * CirclingDirection;
            AddMovementInput(MoveDirection);
            break;

        case EBossAIState::Approaching:
            GetCharacterMovement()->MaxWalkSpeed = SprintSpeed; // 돌진 속도 적용
            MoveDirection = (PlayerCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
            AddMovementInput(MoveDirection);
            break;

        case EBossAIState::BackingOff:
            GetCharacterMovement()->MaxWalkSpeed = WalkSpeed; // 뒷걸음질 속도 적용
            MoveDirection = (GetActorLocation() - PlayerCharacter->GetActorLocation()).GetSafeNormal();
            AddMovementInput(MoveDirection);
            // 일정 시간 후퇴 후, 반드시 관망 상태로 전환하여 다시 생각하게 만듭니다.
            if (!GetWorldTimerManager().IsTimerActive(DecisionTimer))
            {
                 GetWorldTimerManager().SetTimer(DecisionTimer, [this](){
                    CurrentAIState = EBossAIState::Watching;
                    MakeDecision(); // 관망 후 바로 다음 결정
                }, RetreatDuration, false);
            }
            break;

        case EBossAIState::Attacking:
            GetCharacterMovement()->StopMovementImmediately();
            // TODO: 여기에 실제 공격 로직 (애니메이션 몽타주 재생 등)을 추가해야 합니다.
            
            // 임시: 공격 후 바로 후퇴하도록 설정
            CurrentAIState = EBossAIState::BackingOff;
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