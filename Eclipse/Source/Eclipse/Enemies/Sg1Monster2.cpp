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
#include "Player/PlayerCharacter.h"
#include "Gameplay/RiposteDamageType.h"
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
	MonsterAttackDamage = 15.f; // ���� ���� ������ �ʱ�ȭ

	// ������ �ݸ��� �ڽ� ����
	RightHandCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandCollisionBox"));
	RightHandCollisionBox->SetupAttachment(GetMesh(), FName("hand_rSocket")); // ���� ���̷����� ������ ���� �̸����� ���� �ʿ�
	RightHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); // �⺻������ ��Ȱ��ȭ
	RightHandCollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightHandCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightHandCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap); // �÷��̾�͸� ������

	// ������ �ݸ��� �ڽ� ����
	LeftHandCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHandCollisionBox"));
	LeftHandCollisionBox->SetupAttachment(GetMesh(), FName("hand_lSocket")); // ���� ���̷����� ������ ���� �̸����� ���� �ʿ�
	LeftHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); // �⺻������ ��Ȱ��ȭ
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
	// �ݸ��� �ڽ� ������ �̺�Ʈ ���ε�
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
	// ���� �ڽŰ� �浹���� �ʵ���
	if (OtherActor == this) return;

	// �÷��̾� ĳ���Ϳ� �浹�ߴ��� Ȯ��
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		// �̹� ���ظ� �� �������� Ȯ�� (�� ���� ���ݿ� ���� �� ���ظ� ���� �ʵ���)
		if (!HitActors.Contains(OtherActor))
		{
			UGameplayStatics::ApplyDamage(PlayerCharacter, MonsterAttackDamage, GetController(), this, UDamageType::StaticClass());
			HitActors.Add(OtherActor); // ���ظ� �� ���� ��Ͽ� �߰�
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
	// ���� �ݸ��� Ȱ��ȭ
	if (RightHandCollisionBox)
	{
		RightHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		HitActors.Empty(); // ���ο� ���� ���� �� HitActors �ʱ�ȭ
	}
}

void ASg1Monster2::Attack2HitNotify()
{
	// ���� �ݸ��� Ȱ��ȭ
	if (LeftHandCollisionBox)
	{
		LeftHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		HitActors.Empty(); // ���ο� ���� ���� �� HitActors �ʱ�ȭ
	}
}

void ASg1Monster2::AttackEndNotify()
{
	// ���� �ݸ��� ��Ȱ��ȭ
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

	// ��Ÿ�� �迭���� �����ϰ� ����
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
		// ��Ÿ�� ���� �� ���� ����
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
	// ��Ÿ�ְ� ������ ��� ���͸� ����� ��� �浹�� ��Ȱ��ȭ�Ͽ� ���� ����� ����
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	// �����ϰ� �Ҹ�ǵ��� ���� ª�� LifeSpan ����
	SetLifeSpan(0.1f);
}

float ASg1Monster2::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (MonsterState == EMonster2State::EMS_Dead) return DamageAmount;

	// �и��� ���¿����� ������Ʈ �������� ���
	if (MonsterState == EMonster2State::EMS_Parried)
	{
		if (DamageEvent.DamageTypeClass && DamageEvent.DamageTypeClass->IsChildOf(URiposteDamageType::StaticClass()))
		{
			// ������Ʈ �������� ü���� ���ҽ�Ŵ
			Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);
			if (Health <= 0.f)
			{
				Die();
			}
			return DamageAmount;
		}
		else
		{
			// �ٸ� ��� �������� ����
			return 0.f;
		}
	}

	Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);
	if (Health <= 0.f)
	{
		Die();
		return DamageAmount;
	}

	// ������ ���� ���̴� '���� ����' Ÿ�̸Ӱ� �ִٸ� ���.
	GetWorldTimerManager().ClearTimer(StunResetTimerHandle);

	// ���� ���̾��ٸ� ���� Ÿ�̸ӵ� ���.
	GetWorldTimerManager().ClearTimer(AttackTimerHandle);

	// ���¸� Stunned�� ���� (�̹� Stunned ���¿�� �ٽ� ����)
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
		// �ǰ� ��Ÿ�ָ� ó������ �ٽ� ��� (������ ������̾��ٸ� �ߴ��ϰ� ���� ����)
		const float MontageLength = AnimInstance->Montage_Play(HitMontage, 1.0f);

		// ���ο� '���� ����' Ÿ�̸� ����
		GetWorld()->GetTimerManager().SetTimer(StunResetTimerHandle, this, &ASg1Monster2::ResetState, MontageLength, false);
	}
	else
	{
		// ��Ÿ�ְ� ������ �ٷ� ���� ����
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
		return; // ���� ��Ÿ���� �� �������� ����

	LastSidestepTime = CurrentTime;

	float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());

	if (Distance >= 500.f && Distance <= 700.f)
	{
		// �÷��̾� ��ġ�� �������� ���� ���� ���� ���ϱ�
		FVector ToPlayer = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		FVector LeftDirection = FVector::CrossProduct(FVector::UpVector, ToPlayer); // ������ ��ǥ�� ���� ����

		FVector SidestepTarget = GetActorLocation() + LeftDirection * 100.f; // ������ 100���� �̵�

		FRotator LookAtRot = (Player->GetActorLocation() - GetActorLocation()).Rotation();
		SetActorRotation(FRotator(0.f, LookAtRot.Yaw, 0.f)); // �÷��̾� �ٶ󺸱�

		// �̵� ����
		AIController->MoveToLocation(SidestepTarget);

		MonsterState = EMonster2State::EMS_Sidestep;

		// �ʿ��ϸ� �ִϸ��̼ǵ� ���
		if (SidestepMontage)
		{
			UAnimInstance* Anim = GetMesh()->GetAnimInstance();
			if (Anim)
			{
				Anim->Montage_Play(SidestepMontage);
			}
		}
		
		// ���� ���� ����
		GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ASg1Monster2::ResetState, 1.5f, false);
	}
}