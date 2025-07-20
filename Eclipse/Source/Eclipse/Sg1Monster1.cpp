#include "Sg1Monster1.h"
#include "Perception/PawnSensingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "PlayerCharacter.h"

// Sets default values
ASg1Monster1::ASg1Monster1()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SetPeripheralVisionAngle(90.0f);
	PawnSensingComp->SightRadius = 2000.0f; // 기본 시야 반경
    UE_LOG(LogTemp, Warning, TEXT("ASg1Monster1 Constructor Called!"));
}

// Called when the game starts or when spawned
void ASg1Monster1::BeginPlay()
{
	Super::BeginPlay();
	
    UE_LOG(LogTemp, Warning, TEXT("ASg1Monster1 BeginPlay Called!"));
	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &ASg1Monster1::OnSeePawn);
		PawnSensingComp->OnHearNoise.AddDynamic(this, &ASg1Monster1::OnHearNoise);
        UE_LOG(LogTemp, Warning, TEXT("PawnSensingComp events bound."));
	}

	HomeLocation = GetActorLocation(); // 초기 위치 저장
    UE_LOG(LogTemp, Warning, TEXT("HomeLocation: %s"), *HomeLocation.ToString());
	SetState(EMonsterState::Patrol);
}

// Called every frame
void ASg1Monster1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    // UE_LOG(LogTemp, Warning, TEXT("ASg1Monster1 Tick Called! CurrentState: %d"), (int)CurrentState);

	switch (CurrentState)
	{
	case EMonsterState::Patrol:
		HandlePatrolState(DeltaTime);
		break;
	case EMonsterState::Chase:
		HandleChaseState(DeltaTime);
		break;
	case EMonsterState::Attack:
		HandleAttackState(DeltaTime);
		break;
	case EMonsterState::Dead:
		// Dead state logic (e.g., play death animation, disable collision)
		break;
	}
}

// Called to bind functionality to input
void ASg1Monster1::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASg1Monster1::OnSeePawn(APawn* Pawn)
{
    UE_LOG(LogTemp, Warning, TEXT("OnSeePawn Called! Seen: %s"), *Pawn->GetName());
	if (CurrentState == EMonsterState::Dead) return;

	APlayerCharacter* Player = Cast<APlayerCharacter>(Pawn);
	if (Player)
	{
        // 플레이어를 봤을 때, 마지막으로 본 시간을 현재 게임 시간으로 업데이트합니다.
        LastSeenTime = GetWorld()->GetTimeSeconds();

		TargetPawn = Player;
		SetState(EMonsterState::Chase);
        UE_LOG(LogTemp, Warning, TEXT("Player seen, switching to CHASE state."));
	}
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Pawn seen is not APlayerCharacter. Class: %s"), *Pawn->GetClass()->GetName());
    }
}

void ASg1Monster1::OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume)
{
    UE_LOG(LogTemp, Warning, TEXT("OnHearNoise Called! Instigator: %s, Location: %s"), *PawnInstigator->GetName(), *Location.ToString());
	if (CurrentState == EMonsterState::Dead) return;

	// 소음 감지 시 추격 상태로 전환 (플레이어 위치로 이동)
	if (CurrentState != EMonsterState::Chase)
	{
		// TargetPawn = PawnInstigator; // 소음 발생시킨 대상을 추격할 경우
		// SetState(EMonsterState::Chase);
	}
}

void ASg1Monster1::SetState(EMonsterState NewState)
{
    UE_LOG(LogTemp, Warning, TEXT("SetState Called! NewState: %d"), (int)NewState);
	CurrentState = NewState;

	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("AIController is NULL in SetState!"));
        return;
    }

	switch (CurrentState)
	{
	case EMonsterState::Patrol:
		GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
		AIController->StopMovement();
		FindNewPatrolLocation();
        UE_LOG(LogTemp, Warning, TEXT("Switched to PATROL state. Calling FindNewPatrolLocation."));
		break;
	case EMonsterState::Chase:
		GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
		GetWorldTimerManager().ClearTimer(PatrolTimerHandle);
        UE_LOG(LogTemp, Warning, TEXT("Switched to CHASE state."));
		break;
	case EMonsterState::Attack:
		AIController->StopMovement();
		// 공격 애니메이션 재생 로직 추가
        UE_LOG(LogTemp, Warning, TEXT("Switched to ATTACK state."));
		break;
	case EMonsterState::Dead:
		AIController->StopMovement();
		GetWorldTimerManager().ClearTimer(PatrolTimerHandle);
		// 죽음 애니메이션 재생 로직 추가
        UE_LOG(LogTemp, Warning, TEXT("Switched to DEAD state."));
		break;
	}
}

void ASg1Monster1::HandlePatrolState(float DeltaTime)
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController) return;

	// 순찰 목표 지점에 도달했는지 확인
	if (FVector::Dist(GetActorLocation(), PatrolTargetLocation) < 100.0f) // 100.0f는 허용 오차
	{
        UE_LOG(LogTemp, Warning, TEXT("Reached PatrolTargetLocation: %s. Waiting..."), *PatrolTargetLocation.ToString());
		// 목표 지점에 도달했으면 잠시 대기 후 새로운 순찰 위치 탐색
		AIController->StopMovement();
		CurrentPatrolWaitTime -= DeltaTime;
		if (CurrentPatrolWaitTime <= 0.0f)
		{
			FindNewPatrolLocation();
		}
	}
	else
	{
		// 목표 지점으로 이동
		AIController->MoveToLocation(PatrolTargetLocation);
        // UE_LOG(LogTemp, Warning, TEXT("Moving to PatrolTargetLocation: %s"), *PatrolTargetLocation.ToString());
	}
}

void ASg1Monster1::HandleChaseState(float DeltaTime)
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("AIController is NULL in HandleChaseState!"));
        return;
    }
	if (!TargetPawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("TargetPawn is NULL in HandleChaseState! Switching to Patrol."));
        SetState(EMonsterState::Patrol);
        return;
    }

	float DistanceToTarget = FVector::Dist(GetActorLocation(), TargetPawn->GetActorLocation());
    UE_LOG(LogTemp, Warning, TEXT("Chase State: Distance to Target (%s): %f"), *TargetPawn->GetName(), DistanceToTarget);

	// 플레이어가 공격 범위 내에 있으면 공격 상태로 전환
	if (DistanceToTarget <= AttackRange)
	{
		SetState(EMonsterState::Attack);
        UE_LOG(LogTemp, Warning, TEXT("Target in AttackRange. Switching to ATTACK state."));
	}
    // 마지막으로 본 시간부터 ForgetTime(예: 3초)이 지났으면 순찰 상태로 복귀
    else if (GetWorld()->GetTimeSeconds() - LastSeenTime > ForgetTime)
    {
        TargetPawn = nullptr;
        SetState(EMonsterState::Patrol);
        UE_LOG(LogTemp, Warning, TEXT("Target lost for too long. Switching to PATROL state."));
    }
	// 플레이어를 추격
	else
	{
		AIController->MoveToActor(TargetPawn, AttackRange * 0.8f); // 공격 범위 약간 밖에서 멈추도록
        // UE_LOG(LogTemp, Warning, TEXT("Moving to Target: %s"), *TargetPawn->GetName());
	}
}

void ASg1Monster1::HandleAttackState(float DeltaTime)
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("AIController is NULL in HandleAttackState!"));
        return;
    }
	if (!TargetPawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("TargetPawn is NULL in HandleAttackState! Switching to Patrol."));
        SetState(EMonsterState::Patrol);
        return;
    }

	float DistanceToTarget = FVector::Dist(GetActorLocation(), TargetPawn->GetActorLocation());
    UE_LOG(LogTemp, Warning, TEXT("Attack State: Distance to Target (%s): %f"), *TargetPawn->GetName(), DistanceToTarget);

	// 플레이어가 공격 범위 밖으로 나가면 추격 상태로 전환
	if (DistanceToTarget > AttackRange)
	{
		SetState(EMonsterState::Chase);
        UE_LOG(LogTemp, Warning, TEXT("Target out of AttackRange. Switching to CHASE state."));
	}
	// 플레이어가 죽었거나 유효하지 않으면 순찰 상태로 복귀
	else if (!TargetPawn->IsValidLowLevelFast()) // 플레이어가 유효하지 않은 경우
	{
		TargetPawn = nullptr;
		SetState(EMonsterState::Patrol);
        UE_LOG(LogTemp, Warning, TEXT("Target is invalid. Switching to PATROL state."));
	}
	else
	{
		// 플레이어를 바라보도록 회전
		FVector Direction = (TargetPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), Direction.Rotation(), DeltaTime, 10.0f));
        // UE_LOG(LogTemp, Warning, TEXT("Attacking Target: %s"), *TargetPawn->GetName());

		// 공격 로직 (예: 애니메이션 재생, 데미지 적용)
		// 이 부분은 애니메이션 몽타주 재생 등으로 대체될 수 있습니다.
	}
}

void ASg1Monster1::FindNewPatrolLocation()
{
    UE_LOG(LogTemp, Warning, TEXT("FindNewPatrolLocation Called!"));
	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("AIController is NULL in FindNewPatrolLocation!"));
        return;
    }

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSys)
	{
		FNavLocation RandomLocation;
		// HomeLocation 주변 PatrolRadius 내에서 랜덤한 위치 찾기
		if (NavSys->GetRandomPointInNavigableRadius(HomeLocation, PatrolRadius, RandomLocation))
		{
			PatrolTargetLocation = RandomLocation.Location;
			CurrentPatrolWaitTime = PatrolWaitTime; // 대기 시간 초기화
			AIController->MoveToLocation(PatrolTargetLocation);
            UE_LOG(LogTemp, Warning, TEXT("Found new patrol location: %s"), *PatrolTargetLocation.ToString());
		}
		else
		{
			// 랜덤 위치를 찾지 못하면 현재 위치에서 대기 후 다시 시도
			CurrentPatrolWaitTime = PatrolWaitTime; 
            UE_LOG(LogTemp, Warning, TEXT("Failed to find random patrol location. Waiting to retry."));
		}
	}
    else
    {
        UE_LOG(LogTemp, Error, TEXT("NavigationSystem is NULL in FindNewPatrolLocation!"));
    }
}