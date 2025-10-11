
#include "World2AIBossCharacter.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/UnrealMathUtility.h"

AWorld2AIBossCharacter::AWorld2AIBossCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = false;

    // 초기 상태를 Idle로 설정합니다.
    CurrentMovementState = EBossMovementState::Idle;
}

void AWorld2AIBossCharacter::BeginPlay()
{
    Super::BeginPlay();

    PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    if (!PlayerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("World2AIBossCharacter: PlayerCharacter not found!"));
    }

    // 게임이 시작되면 첫 상태 업데이트를 즉시 실행합니다.
    UpdateMovementState();
}

void AWorld2AIBossCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 플레이어 캐릭터가 유효한지 확인합니다.
    if (PlayerCharacter)
    {
        // 플레이어를 계속 바라보도록 합니다.
        FacePlayer(DeltaTime);
        // 플레이어의 움직임에 맞춰 심리전 이동을 수행합니다.
        PsychologicalMove(DeltaTime);
    }
}

void AWorld2AIBossCharacter::FacePlayer(float DeltaTime)
{
    if (!PlayerCharacter) return;

    // 보스에서 플레이어로 향하는 방향 벡터를 계산합니다.
    const FVector DirectionToPlayer = PlayerCharacter->GetActorLocation() - GetActorLocation();
    // Z축 값은 무시하여 수평으로만 바라보게 합니다.
    const FRotator TargetRotation = FRotator(0.0f, DirectionToPlayer.Rotation().Yaw, 0.0f);

    // 현재 회전 값에서 목표 회전 값으로 부드럽게 전환합니다.
    const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, RotationSpeed);

    SetActorRotation(NewRotation);
}

void AWorld2AIBossCharacter::PsychologicalMove(float DeltaTime)
{
    if (!PlayerCharacter || CurrentMovementState == EBossMovementState::Idle)
    {
        // 플레이어가 없거나, 현재 상태가 'Idle'이면 아무것도 하지 않습니다.
        return;
    }

    // --- 좌우 이동 로직 (기존과 유사) ---
    FVector PlayerMovementDirection = PlayerCharacter->GetLastMovementInputVector();
    FVector PlayerRightVector = PlayerCharacter->GetActorRightVector();
    float PlayerStrafeAmount = FVector::DotProduct(PlayerMovementDirection, PlayerRightVector);
    FVector StrafeDirection = GetActorRightVector() * -PlayerStrafeAmount;

    // --- 거리 조절 로직 (수정됨) ---
    FVector DirectionToPlayer = PlayerCharacter->GetActorLocation() - GetActorLocation();
    float DistanceToPlayer = DirectionToPlayer.Size2D(); // 수평 거리만 계산

    FVector DistanceControlDirection = FVector::ZeroVector;
    if (DistanceToPlayer > MaxIdealDistance)
    {
        // 최대 거리보다 멀어지면 다가갑니다.
        DistanceControlDirection = DirectionToPlayer.GetSafeNormal();
    }
    else if (DistanceToPlayer < MinIdealDistance)
    { 
        // 최소 거리보다 가까워지면 물러납니다.
        DistanceControlDirection = -DirectionToPlayer.GetSafeNormal();
    }
    // 참고: 이상적인 거리 내에 있을 경우, 거리 조절을 위한 움직임은 없습니다.

    // 최종 이동 방향을 계산합니다.
    FVector FinalDirection = StrafeDirection + DistanceControlDirection;

    // 이동 방향이 있을 경우에만 움직임을 추가합니다.
    if (!FinalDirection.IsNearlyZero())
    {
        AddMovementInput(FinalDirection.GetSafeNormal(), 1.0f);
    }
}

void AWorld2AIBossCharacter::UpdateMovementState()
{
    if (!PlayerCharacter)
    {
        // 플레이어가 없으면 상태를 Idle로 고정하고 타이머를 재설정하지 않습니다.
        CurrentMovementState = EBossMovementState::Idle;
        GetCharacterMovement()->MaxWalkSpeed = 0;
        return;
    }

    float DistanceToPlayer = GetDistanceTo(PlayerCharacter);
    int32 RandomChoice = FMath::RandRange(0, 100);

    // 다음 상태를 결정합니다.
    if (DistanceToPlayer > MaxIdealDistance + 200.f)
    {
        // 플레이어가 너무 멀리 있으면, 80% 확률로 뛰고 20% 확률로 걷습니다.
        CurrentMovementState = (RandomChoice < 80) ? EBossMovementState::Sprinting : EBossMovementState::Walking;
    }
    else if (DistanceToPlayer < MinIdealDistance - 100.f)
    {
        // 플레이어가 너무 가까우면, 50% 확률로 뛰어서 물러나고 50% 확률로 걸어서 물러납니다.
        CurrentMovementState = (RandomChoice < 50) ? EBossMovementState::Sprinting : EBossMovementState::Walking;
    }
    else
    {
        // 이상적인 거리 내에 있으면, 60% 확률로 가만히 있고 40% 확률로 걷습니다.
        CurrentMovementState = (RandomChoice < 60) ? EBossMovementState::Idle : EBossMovementState::Walking;
    }

    // 결정된 상태에 따라 이동 속도를 설정합니다.
    switch (CurrentMovementState)
    {
        case EBossMovementState::Idle:
            GetCharacterMovement()->MaxWalkSpeed = 0;
            break;
        case EBossMovementState::Walking:
            GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
            break;
        case EBossMovementState::Sprinting:
            GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
            break;
    }

    // 다음 상태 변경까지의 시간을 랜덤하게 설정하고 타이머를 다시 시작합니다.
    float NextStateChangeDelay = FMath::RandRange(MinStateChangeTime, MaxStateChangeTime);
    GetWorldTimerManager().SetTimer(StateChangeTimer, this, &AWorld2AIBossCharacter::UpdateMovementState, NextStateChangeDelay, false);
}
