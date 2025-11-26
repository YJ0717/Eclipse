#include "Sg2Monster4.h"
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

ASg2Monster4::ASg2Monster4()
{
	PrimaryActorTick.bCanEverTick = true;
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SensingInterval = 0.5f;
	PawnSensingComp->SetPeripheralVisionAngle(45.f);
	MaxHealth = 150.f;
	Health = MaxHealth;
	PatrolRadius = 1500.f;
	ChaseTimeout = 5.0f;
	MonsterState = EMonster4State::EMS_Patrolling;
	MonsterAttackDamage = 15.f; 

	BiteCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("BiteCollisionSphere"));
	BiteCollisionSphere->SetupAttachment(GetMesh(), FName("headSocket")); // 입 부분 소켓 이름
	BiteCollisionSphere->InitSphereRadius(30.f); // 크기 조절
	BiteCollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BiteCollisionSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BiteCollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BiteCollisionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	

}

void ASg2Monster4::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
	AIController = Cast<AAIController>(GetController());
	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &ASg2Monster4::OnPawnSeen);
	}
	
	if (BiteCollisionSphere)
	{
		BiteCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ASg2Monster4::OnAttackOverlapBegin);
	}

	
}

void ASg2Monster4::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateAIState();
}

void ASg2Monster4::UpdateAIState()
{
	if (MonsterState == EMonster4State::EMS_Stunned || MonsterState == EMonster4State::EMS_Dead || MonsterState == EMonster4State::EMS_Attacking || MonsterState == EMonster4State::EMS_Parried)
	{
		return;
	}
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!Player) return;

	
	if (MonsterState == EMonster4State::EMS_Chasing)
	{
		Chase();
	}
	else if (MonsterState == EMonster4State::EMS_Patrolling)
	{
		Patrol();
	}
}

void ASg2Monster4::OnPawnSeen(APawn* SeenPawn)
{
	if (SeenPawn == nullptr || MonsterState == EMonster4State::EMS_Dead) return;
	if (UGameplayStatics::GetPlayerCharacter(GetWorld(), 0) == SeenPawn)
	{
		LastSeenTime = GetWorld()->GetTimeSeconds();
		if (MonsterState == EMonster4State::EMS_Patrolling)
		{
			MonsterState = EMonster4State::EMS_Chasing;
			GetWorldTimerManager().ClearTimer(AttackTimerHandle);
		}
	}
}

void ASg2Monster4::OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (OtherActor == this) return;

	
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		
		if (!HitActors.Contains(OtherActor))
		{
			UGameplayStatics::ApplyDamage(PlayerCharacter, MonsterAttackDamage, GetController(), this, UDamageType::StaticClass());
			HitActors.Add(OtherActor); 
		}
	}
}

void ASg2Monster4::Patrol()
{
	if (!AIController || MonsterState == EMonster4State::EMS_Dead) return;
	if (AIController->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		MoveToRandomLocation();
	}
}

void ASg2Monster4::MoveToRandomLocation()
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

void ASg2Monster4::Chase()
{
	if (!AIController || MonsterState == EMonster4State::EMS_Dead) return;
	APawn* PlayerPawn = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerPawn)
	{
		if (GetDistanceTo(PlayerPawn) <= 200.f)
		{
			MonsterState = EMonster4State::EMS_Attacking;
			Attack();
		}
		else
		{
			AIController->MoveToActor(PlayerPawn);
		}
	}
	if (GetWorld()->GetTimeSeconds() - LastSeenTime > ChaseTimeout)
	{
		MonsterState = EMonster4State::EMS_Patrolling;
	}
}

void ASg2Monster4::AttackHitNotify()
{
	
	if (BiteCollisionSphere)
	{
		BiteCollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		HitActors.Empty();
	}
}



void ASg2Monster4::AttackEndNotify()
{
	
	if (BiteCollisionSphere)
	{
		BiteCollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
}
void ASg2Monster4::Attack()
{
	if (!AIController || MonsterState == EMonster4State::EMS_Dead) return;
	AIController->StopMovement();

	
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
					if (MonsterState != EMonster4State::EMS_Dead && MonsterState != EMonster4State::EMS_Parried)
					{
						MonsterState = EMonster4State::EMS_Chasing;
					}
				});

			GetWorldTimerManager().SetTimer(AttackTimerHandle, TimerDelegate, MontageLength, false);
		}
	}
	else
	{
		
		if (MonsterState != EMonster4State::EMS_Dead && MonsterState != EMonster4State::EMS_Parried)
		{
			MonsterState = EMonster4State::EMS_Chasing;
		}
	}
}
void ASg2Monster4::Die()
{
	MonsterState = EMonster4State::EMS_Dead;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(this, &ASg2Monster4::OnDeathMontageEnded);
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, DeathMontage);
	}
	else
	{
		Destroy();
	}
}

void ASg2Monster4::OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	
	SetLifeSpan(0.1f);
}

float ASg2Monster4::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (MonsterState == EMonster4State::EMS_Dead) return DamageAmount;


	if (MonsterState == EMonster4State::EMS_Parried)
	{
		if (DamageEvent.DamageTypeClass && DamageEvent.DamageTypeClass->IsChildOf(URiposteDamageType::StaticClass()))
		{
			
			Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);
			if (Health <= 0.f)
			{
				Die();
			}
			return DamageAmount;
		}
		else
		{
			
			return 0.f;
		}
	}

	Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);
	if (Health <= 0.f)
	{
		Die();
		return DamageAmount;
	}

	
	GetWorldTimerManager().ClearTimer(StunResetTimerHandle);

	
	GetWorldTimerManager().ClearTimer(AttackTimerHandle);

	
	MonsterState = EMonster4State::EMS_Stunned;

	OnDamaged_BP();

	if (AIController)
	{
		AIController->StopMovement();
	}
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitMontage)
	{
		
		const float MontageLength = AnimInstance->Montage_Play(HitMontage, 1.0f);

		
		GetWorld()->GetTimerManager().SetTimer(StunResetTimerHandle, this, &ASg2Monster4::ResetState, MontageLength, false);
	}
	else
	{
		
		ResetState();
	}

	return DamageAmount;
}

void ASg2Monster4::ResetState()
{
	if (MonsterState == EMonster4State::EMS_Dead) return;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	if (GetWorld()->GetTimeSeconds() - LastSeenTime <= ChaseTimeout)
	{
		MonsterState = EMonster4State::EMS_Chasing;
	}
	else
	{
		MonsterState = EMonster4State::EMS_Patrolling;
	}
}


void ASg2Monster4::GetParried()
{
	if (MonsterState == EMonster4State::EMS_Dead) return;

	MonsterState = EMonster4State::EMS_Parried;

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

	GetWorldTimerManager().SetTimer(StaggerTimerHandle, this, &ASg2Monster4::RecoverFromStagger, 3.0f, false);
}

void ASg2Monster4::RecoverFromStagger()
{
	ResetState();
}

bool ASg2Monster4::IsStaggered() const
{
	return MonsterState == EMonster4State::EMS_Parried;
}
