#include "Sg1Monster1.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Navigation/PathFollowingComponent.h"

ASg1Monster1::ASg1Monster1()
{
	PrimaryActorTick.bCanEverTick = true;
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SensingInterval = 0.5f;
	PawnSensingComp->SetPeripheralVisionAngle(45.f);
	MaxHealth = 100.f;
	Health = MaxHealth;
	PatrolRadius = 1500.f;
	ChaseTimeout = 5.0f;
	MonsterState = EMonsterState::EMS_Patrolling;
}

void ASg1Monster1::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
	AIController = Cast<AAIController>(GetController());
	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &ASg1Monster1::OnPawnSeen);
	}
	MoveToRandomLocation();
}

void ASg1Monster1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateAIState();
}

void ASg1Monster1::UpdateAIState()
{
	if (MonsterState == EMonsterState::EMS_Stunned || MonsterState == EMonsterState::EMS_Dead || MonsterState == EMonsterState::EMS_Attacking)
	{
		return;
	}
	if (MonsterState == EMonsterState::EMS_Chasing)
	{
		Chase();
	}
	else if (MonsterState == EMonsterState::EMS_Patrolling)
	{
		Patrol();
	}
}

void ASg1Monster1::OnPawnSeen(APawn* SeenPawn)
{
	if (SeenPawn == nullptr || MonsterState == EMonsterState::EMS_Dead) return;
	if (UGameplayStatics::GetPlayerCharacter(GetWorld(), 0) == SeenPawn)
	{
		LastSeenTime = GetWorld()->GetTimeSeconds();
		if (MonsterState == EMonsterState::EMS_Patrolling)
		{
			MonsterState = EMonsterState::EMS_Chasing;
			GetWorldTimerManager().ClearTimer(AttackTimerHandle);
		}
	}
}

void ASg1Monster1::Patrol()
{
	if (!AIController || MonsterState == EMonsterState::EMS_Dead) return;
	if (AIController->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		MoveToRandomLocation();
	}
}

void ASg1Monster1::MoveToRandomLocation()
{
	if (!AIController) return;
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem)
	{
		FNavLocation RandomLocation;
		bool bFound = NavSystem->GetRandomReachablePointInRadius(GetActorLocation(), PatrolRadius, RandomLocation);
		if (bFound)
		{
			AIController->MoveToLocation(RandomLocation.Location);
		}
	}
}

void ASg1Monster1::Chase()
{
	if (!AIController || MonsterState == EMonsterState::EMS_Dead) return;
	APawn* PlayerPawn = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerPawn)
	{
		if (GetDistanceTo(PlayerPawn) <= 200.f)
		{
			MonsterState = EMonsterState::EMS_Attacking;
			Attack();
		}
		else
		{
			AIController->MoveToActor(PlayerPawn);
		}
	}
	if (GetWorld()->GetTimeSeconds() - LastSeenTime > ChaseTimeout)
	{
		MonsterState = EMonsterState::EMS_Patrolling;
	}
}

void ASg1Monster1::Attack()
{
	if (!AIController || MonsterState == EMonsterState::EMS_Dead) return;
	AIController->StopMovement();
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		const float MontageLength = AnimInstance->Montage_Play(AttackMontage);
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([&]()
		{
			if(MonsterState != EMonsterState::EMS_Dead) MonsterState = EMonsterState::EMS_Chasing;
		});
		GetWorldTimerManager().SetTimer(AttackTimerHandle, TimerDelegate, MontageLength, false);
	}
	else
	{
		if(MonsterState != EMonsterState::EMS_Dead) MonsterState = EMonsterState::EMS_Chasing;
	}
}

void ASg1Monster1::Die()
{
	MonsterState = EMonsterState::EMS_Dead;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(this, &ASg1Monster1::OnDeathMontageEnded);
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, DeathMontage);
	}
	else
	{
		Destroy();
	}
}

void ASg1Monster1::OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 몽타주가 끝나는 즉시 액터를 숨기고 모든 충돌을 비활성화하여 없는 존재로 만듦
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	// 안전하게 소멸되도록 아주 짧은 LifeSpan 설정
	SetLifeSpan(0.1f);
}

float ASg1Monster1::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (MonsterState == EMonsterState::EMS_Dead)
	{
		return DamageAmount;
	}

	Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);
	if (Health <= 0.f)
	{
		Die();
		return DamageAmount;
	}

	// 기존에 실행 중이던 '경직 해제' 타이머가 있다면 취소.
	GetWorldTimerManager().ClearTimer(StunResetTimerHandle);
	
	// 공격 중이었다면 공격 타이머도 취소.
	GetWorldTimerManager().ClearTimer(AttackTimerHandle);

	// 상태를 Stunned로 설정 (이미 Stunned 상태였어도 다시 설정)
	MonsterState = EMonsterState::EMS_Stunned;
	
	if (AIController)
	{
		AIController->StopMovement();
	}
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitMontage)
	{
		// 피격 몽타주를 처음부터 다시 재생 (기존에 재생중이었다면 중단하고 새로 시작)
		const float MontageLength = AnimInstance->Montage_Play(HitMontage, 1.0f);
		
		// 새로운 '경직 해제' 타이머 설정
		GetWorld()->GetTimerManager().SetTimer(StunResetTimerHandle, this, &ASg1Monster1::ResetState, MontageLength, false);
	}
	else
	{
		// 몽타주가 없으면 바로 상태 리셋
		ResetState();
	}

	return DamageAmount;
}

void ASg1Monster1::ResetState()
{
	if (MonsterState == EMonsterState::EMS_Dead) return;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	if (GetWorld()->GetTimeSeconds() - LastSeenTime <= ChaseTimeout)
	{
		MonsterState = EMonsterState::EMS_Chasing;
	}
	else
	{
		MonsterState = EMonsterState::EMS_Patrolling;
	}
}