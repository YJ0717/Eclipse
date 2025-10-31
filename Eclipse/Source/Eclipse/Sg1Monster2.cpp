#include "Sg1Monster2.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "PlayerCharacter.h"
#include "RiposteDamageType.h"
#include "Engine/DamageEvents.h"

ASg1Monster2::ASg1Monster2()
{
	PrimaryActorTick.bCanEverTick = true;
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SensingInterval = 0.5f;
	PawnSensingComp->SetPeripheralVisionAngle(45.f);
	MaxHealth = 150.f;
	Health = MaxHealth;
	PatrolRadius = 1500.f;
	ChaseTimeout = 5.0f;
	MonsterState = EMonster2State::EMS_Patrolling;
	MonsterAttackDamage = 15.f; // 몬스터 공격 데미지 초기화

	// 오른손 콜리전 박스 설정
	RightHandCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandCollisionBox"));
	RightHandCollisionBox->SetupAttachment(GetMesh(), FName("hand_rSocket")); // 몬스터 스켈레톤의 오른손 소켓 이름으로 변경 필요
	RightHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 기본적으로 비활성화
	RightHandCollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightHandCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightHandCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap); // 플레이어와만 오버랩

	// 오른손 콜리전 박스 설정
	LeftHandCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHandCollisionBox"));
	LeftHandCollisionBox->SetupAttachment(GetMesh(), FName("hand_lSocket")); // 몬스터 스켈레톤의 오른손 소켓 이름으로 변경 필요
	LeftHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 기본적으로 비활성화
	LeftHandCollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftHandCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftHandCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);


}

void ASg1Monster2::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
	AIController = Cast<AAIController>(GetController());
	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &ASg1Monster2::OnPawnSeen);
	}
	// 콜리전 박스 오버랩 이벤트 바인딩
	if (RightHandCollisionBox)
	{
		RightHandCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ASg1Monster2::OnAttackOverlapBegin);
	}

	if (LeftHandCollisionBox)
	{
		LeftHandCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ASg1Monster2::OnAttackOverlapBegin);
	}
	MoveToRandomLocation();
}

void ASg1Monster2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateAIState();
}

void ASg1Monster2::UpdateAIState()
{
	if (MonsterState == EMonster2State::EMS_Stunned || MonsterState == EMonster2State::EMS_Dead || MonsterState == EMonster2State::EMS_Attacking || MonsterState == EMonster2State::EMS_Parried)
	{
		return;
	}
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!Player) return;

	float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
	if (Distance >= 500.f && Distance <= 700.f)
	{
		Sidestep();
		return;
	}
	if (MonsterState == EMonster2State::EMS_Chasing)
	{
		Chase();
	}
	else if (MonsterState == EMonster2State::EMS_Patrolling)
	{
		Patrol();
	}
}

void ASg1Monster2::OnPawnSeen(APawn* SeenPawn)
{
	if (SeenPawn == nullptr || MonsterState == EMonster2State::EMS_Dead) return;
	if (UGameplayStatics::GetPlayerCharacter(GetWorld(), 0) == SeenPawn)
	{
		LastSeenTime = GetWorld()->GetTimeSeconds();
		if (MonsterState == EMonster2State::EMS_Patrolling)
		{
			MonsterState = EMonster2State::EMS_Chasing;
			GetWorldTimerManager().ClearTimer(AttackTimerHandle);
		}
	}
}

void ASg1Monster2::OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 몬스터 자신과 충돌하지 않도록
	if (OtherActor == this) return;

	// 플레이어 캐릭터와 충돌했는지 확인
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		// 이미 피해를 준 액터인지 확인 (한 번의 공격에 여러 번 피해를 주지 않도록)
		if (!HitActors.Contains(OtherActor))
		{
			UGameplayStatics::ApplyDamage(PlayerCharacter, MonsterAttackDamage, GetController(), this, UDamageType::StaticClass());
			HitActors.Add(OtherActor); // 피해를 준 액터 목록에 추가
		}
	}
}

void ASg1Monster2::Patrol()
{
	if (!AIController || MonsterState == EMonster2State::EMS_Dead) return;
	if (AIController->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		MoveToRandomLocation();
	}
}

void ASg1Monster2::MoveToRandomLocation()
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

void ASg1Monster2::Chase()
{
	if (!AIController || MonsterState == EMonster2State::EMS_Dead) return;
	APawn* PlayerPawn = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerPawn)
	{
		if (GetDistanceTo(PlayerPawn) <= 200.f)
		{
			MonsterState = EMonster2State::EMS_Attacking;
			Attack();
		}
		else
		{
			AIController->MoveToActor(PlayerPawn);
		}
	}
	if (GetWorld()->GetTimeSeconds() - LastSeenTime > ChaseTimeout)
	{
		MonsterState = EMonster2State::EMS_Patrolling;
	}
}

void ASg1Monster2::AttackHitNotify()
{
	// 공격 콜리전 활성화
	if (RightHandCollisionBox)
	{
		RightHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		HitActors.Empty(); // 새로운 공격 시작 시 HitActors 초기화
	}
}

void ASg1Monster2::Attack2HitNotify()
{
	// 공격 콜리전 활성화
	if (LeftHandCollisionBox)
	{
		LeftHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		HitActors.Empty(); // 새로운 공격 시작 시 HitActors 초기화
	}
}

void ASg1Monster2::AttackEndNotify()
{
	// 공격 콜리전 비활성화
	if (RightHandCollisionBox)
	{
		RightHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (LeftHandCollisionBox)
	{
		LeftHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
void ASg1Monster2::Attack()
{
	if (!AIController || MonsterState == EMonster2State::EMS_Dead) return;
	AIController->StopMovement();

	// 몽타주 배열에서 랜덤하게 선택
	if (AttackMontages.Num() > 0)
	{
		int32 Index = FMath::RandRange(0, AttackMontages.Num() - 1);
		UAnimMontage* SelectedMontage = AttackMontages[Index];

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && SelectedMontage)
		{
			const float MontageLength = AnimInstance->Montage_Play(SelectedMontage);

			FTimerDelegate TimerDelegate;
			TimerDelegate.BindLambda([this]()
				{
					if (MonsterState != EMonster2State::EMS_Dead && MonsterState != EMonster2State::EMS_Parried)
					{
						MonsterState = EMonster2State::EMS_Chasing;
					}
				});

			GetWorldTimerManager().SetTimer(AttackTimerHandle, TimerDelegate, MontageLength, false);
		}
	}
	else
	{
		// 몽타주 없음 → 상태 복구
		if (MonsterState != EMonster2State::EMS_Dead && MonsterState != EMonster2State::EMS_Parried)
		{
			MonsterState = EMonster2State::EMS_Chasing;
		}
	}
}
void ASg1Monster2::Die()
{
	MonsterState = EMonster2State::EMS_Dead;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(this, &ASg1Monster2::OnDeathMontageEnded);
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, DeathMontage);
	}
	else
	{
		Destroy();
	}
}

void ASg1Monster2::OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 몽타주가 끝나는 즉시 액터를 숨기고 모든 충돌을 비활성화하여 없는 존재로 만듦
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	// 안전하게 소멸되도록 아주 짧은 LifeSpan 설정
	SetLifeSpan(0.1f);
}

float ASg1Monster2::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (MonsterState == EMonster2State::EMS_Dead) return DamageAmount;

	// 패링된 상태에서는 리포스트 데미지만 허용
	if (MonsterState == EMonster2State::EMS_Parried)
	{
		if (DamageEvent.DamageTypeClass && DamageEvent.DamageTypeClass->IsChildOf(URiposteDamageType::StaticClass()))
		{
			// 리포스트 데미지는 체력을 감소시킴
			Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);
			if (Health <= 0.f)
			{
				Die();
			}
			return DamageAmount;
		}
		else
		{
			// 다른 모든 데미지는 무시
			return 0.f;
		}
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
	MonsterState = EMonster2State::EMS_Stunned;

	OnDamaged_BP();

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
		GetWorld()->GetTimerManager().SetTimer(StunResetTimerHandle, this, &ASg1Monster2::ResetState, MontageLength, false);
	}
	else
	{
		// 몽타주가 없으면 바로 상태 리셋
		ResetState();
	}

	return DamageAmount;
}

void ASg1Monster2::ResetState()
{
	if (MonsterState == EMonster2State::EMS_Dead) return;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	if (GetWorld()->GetTimeSeconds() - LastSeenTime <= ChaseTimeout)
	{
		MonsterState = EMonster2State::EMS_Chasing;
	}
	else
	{
		MonsterState = EMonster2State::EMS_Patrolling;
	}
}


void ASg1Monster2::GetParried()
{
	if (MonsterState == EMonster2State::EMS_Dead) return;

	MonsterState = EMonster2State::EMS_Parried;

	if (AIController)
	{
		AIController->StopMovement();
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->StopAllMontages(0.2f);
		if (StaggerMontage)
		{
			AnimInstance->Montage_Play(StaggerMontage);
		}
	}

	GetWorldTimerManager().SetTimer(StaggerTimerHandle, this, &ASg1Monster2::RecoverFromStagger, 3.0f, false);
}

void ASg1Monster2::RecoverFromStagger()
{
	ResetState();
}

bool ASg1Monster2::IsStaggered() const
{
	return MonsterState == EMonster2State::EMS_Parried;
}

void ASg1Monster2::Sidestep()
{
	if (!AIController || MonsterState == EMonster2State::EMS_Dead) return;

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!Player) return;

	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastSidestepTime < SidestepCooldown)
		return; // 아직 쿨타임이 안 지났으면 무시

	LastSidestepTime = CurrentTime;

	float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());

	if (Distance >= 500.f && Distance <= 700.f)
	{
		// 플레이어 위치를 기준으로 왼쪽 벡터 방향 구하기
		FVector ToPlayer = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		FVector LeftDirection = FVector::CrossProduct(FVector::UpVector, ToPlayer); // 오른손 좌표계 기준 왼쪽

		FVector SidestepTarget = GetActorLocation() + LeftDirection * 100.f; // 옆으로 100유닛 이동

		FRotator LookAtRot = (Player->GetActorLocation() - GetActorLocation()).Rotation();
		SetActorRotation(FRotator(0.f, LookAtRot.Yaw, 0.f)); // 플레이어 바라보기

		// 이동 명령
		AIController->MoveToLocation(SidestepTarget);

		MonsterState = EMonster2State::EMS_Sidestep;

		// 필요하면 애니메이션도 재생
		if (SidestepMontage)
		{
			UAnimInstance* Anim = GetMesh()->GetAnimInstance();
			if (Anim)
			{
				Anim->Montage_Play(SidestepMontage);
			}
		}
		
		// 상태 리셋 예약
		GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ASg1Monster2::ResetState, 1.5f, false);
	}
}