// Fill out your copyright notice in the Description page of Project Settings.

#include "Sg4BossCharacter.h"
#include "Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "Gameplay/BossRoomManager.h"

ASg4BossCharacter::ASg4BossCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f); // 회전 속도 설정

	CurrentAIState = ESg4BossState::Watching;
	bIsAttacking = false;

	CurrentHealth = MaxHealth;

	// 검 충돌 박스 (R_Weapon001 소켓에 부착)
	SwordCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SwordCollisionBox"));
	SwordCollisionBox->SetupAttachment(GetMesh(), FName("R_Weapon001"));
	SwordCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SwordCollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	SwordCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SwordCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void ASg4BossCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	bIsDead = false;

	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (SwordCollisionBox)
	{
		SwordCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ASg4BossCharacter::OnWeaponOverlap);
	}

	GetWorldTimerManager().SetTimer(DecisionTimer, this, &ASg4BossCharacter::MakeDecision, 0.5f, false);
}

void ASg4BossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDead) return;

	// 공격 중에는 스킵
	if (bIsAttacking)
	{
		return;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan,
			FString::Printf(TEXT("Sg4Boss State: %s"), *UEnum::GetValueAsString(CurrentAIState)));
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red,
			FString::Printf(TEXT("Sg4Boss HP: %.1f"), CurrentHealth));
	}

	if (PlayerCharacter)
	{
		FacePlayer(DeltaTime);
		ExecuteState(DeltaTime);
	}
}

void ASg4BossCharacter::FacePlayer(float DeltaTime)
{
	if (!PlayerCharacter || bIsDead) return;

	const FVector DirectionToPlayer = PlayerCharacter->GetActorLocation() - GetActorLocation();
	const FRotator TargetRotation = FRotator(0.0f, DirectionToPlayer.Rotation().Yaw, 0.0f);
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, RotationSpeed));
}

void ASg4BossCharacter::MakeDecision()
{
	if (bIsDead || CurrentAIState == ESg4BossState::MeleeAttacking ||
		CurrentAIState == ESg4BossState::RangedAttacking ||
		CurrentAIState == ESg4BossState::BackingOff ||
		bIsAttacking)
		return;

	float Distance = GetDistanceTo(PlayerCharacter);

	// 근거리: 근접 공격
	if (Distance <= MeleeAttackRange)
	{
		int32 RandVal = FMath::RandRange(0, 100);
		if (RandVal < 70) // 70% 근접 공격
		{
			CurrentAIState = ESg4BossState::MeleeAttacking;
		}
		else if (RandVal < 90) // 20% 선회
		{
			CurrentAIState = ESg4BossState::Circling;
			CirclingDirection = FMath::RandBool() ? 1.0f : -1.0f;
		}
		else // 10% 후퇴
		{
			CurrentAIState = ESg4BossState::BackingOff;
		}
	}
	// 중거리: 원거리 공격
	else if (Distance > MeleeAttackRange && Distance <= RangedAttackRange)
	{
		int32 RandVal = FMath::RandRange(0, 100);
		if (RandVal < 60) // 60% 원거리 공격
		{
			CurrentAIState = ESg4BossState::RangedAttacking;
		}
		else if (RandVal < 80) // 20% 접근
		{
			CurrentAIState = ESg4BossState::Approaching;
		}
		else // 20% 선회
		{
			CurrentAIState = ESg4BossState::Circling;
			CirclingDirection = FMath::RandBool() ? 1.0f : -1.0f;
		}
	}
	// 너무 멀면: 접근
	else
	{
		if (FMath::RandRange(0, 100) < 80)
		{
			CurrentAIState = ESg4BossState::Approaching;
		}
		else
		{
			CurrentAIState = ESg4BossState::Watching;
		}
	}

	GetWorldTimerManager().SetTimer(DecisionTimer, this, &ASg4BossCharacter::MakeDecision, DecisionInterval, false);
}

void ASg4BossCharacter::ExecuteState(float DeltaTime)
{
	if (bIsDead)
	{
		GetCharacterMovement()->StopMovementImmediately();
		return;
	}

	if (CurrentAIState == ESg4BossState::MeleeAttacking ||
		CurrentAIState == ESg4BossState::RangedAttacking ||
		CurrentAIState == ESg4BossState::AttackCooldown)
	{
		GetCharacterMovement()->StopMovementImmediately();

		if (CurrentAIState == ESg4BossState::MeleeAttacking && !bIsAttacking)
		{
			PerformMeleeAttack();
		}
		else if (CurrentAIState == ESg4BossState::RangedAttacking && !bIsAttacking)
		{
			PerformRangedAttack();
		}
	}
	else
	{
		FVector MoveDirection = FVector::ZeroVector;

		switch (CurrentAIState)
		{
		case ESg4BossState::Watching:
			GetCharacterMovement()->MaxWalkSpeed = 0;
			break;

		case ESg4BossState::Circling:
			GetCharacterMovement()->MaxWalkSpeed = StrafeSpeed;
			MoveDirection = GetActorRightVector() * CirclingDirection;
			AddMovementInput(MoveDirection);
			break;

		case ESg4BossState::Approaching:
			GetCharacterMovement()->MaxWalkSpeed = ChargeSpeed;
			MoveDirection = (PlayerCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			AddMovementInput(MoveDirection);
			break;

		case ESg4BossState::BackingOff:
			GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
			MoveDirection = (GetActorLocation() - PlayerCharacter->GetActorLocation()).GetSafeNormal();
			AddMovementInput(MoveDirection);
			if (!GetWorldTimerManager().IsTimerActive(DecisionTimer))
			{
				GetWorldTimerManager().SetTimer(DecisionTimer, [this]() {
					CurrentAIState = ESg4BossState::Watching;
					MakeDecision();
					}, RetreatDuration, false);
			}
			break;
		}
	}
}

void ASg4BossCharacter::PerformMeleeAttack()
{
	if (MeleeAttackMontages.Num() == 0 || bIsDead)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sg4Boss: No melee attack montages! Skipping attack."));
		// 몽타주 없어도 쿨다운 진입
		CurrentAIState = ESg4BossState::AttackCooldown;
		GetWorldTimerManager().SetTimer(DecisionTimer, this, &ASg4BossCharacter::MakeDecision, AttackCooldownDuration, false);
		return;
	}

	bIsAttacking = true;

	int32 Index = FMath::RandRange(0, MeleeAttackMontages.Num() - 1);
	UAnimMontage* AttackToPlay = MeleeAttackMontages[Index];

	if (AttackToPlay)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sg4Boss: Performing MELEE attack '%s'"), *AttackToPlay->GetName());
		PlayAnimMontage(AttackToPlay);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			FOnMontageEnded MontageEndedDelegate;
			MontageEndedDelegate.BindUObject(this, &ASg4BossCharacter::OnAttackMontageEnded);
			AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, AttackToPlay);
		}
	}
}

void ASg4BossCharacter::PerformRangedAttack()
{
	if (RangedAttackMontages.Num() == 0 || bIsDead)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sg4Boss: No ranged attack montages! Skipping attack."));
		// 몽타주 있어도 쿨다운 진입
		CurrentAIState = ESg4BossState::AttackCooldown;
		GetWorldTimerManager().SetTimer(DecisionTimer, this, &ASg4BossCharacter::MakeDecision, AttackCooldownDuration, false);
		return;
	}

	bIsAttacking = true;

	int32 Index = FMath::RandRange(0, RangedAttackMontages.Num() - 1);
	UAnimMontage* AttackToPlay = RangedAttackMontages[Index];

	if (AttackToPlay)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sg4Boss: Performing RANGED attack '%s'"), *AttackToPlay->GetName());
		PlayAnimMontage(AttackToPlay);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			FOnMontageEnded MontageEndedDelegate;
			MontageEndedDelegate.BindUObject(this, &ASg4BossCharacter::OnAttackMontageEnded);
			AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, AttackToPlay);
		}
	}
}

void ASg4BossCharacter::FireMagicProjectile()
{
	if (!MagicProjectileClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Sg4Boss: MagicProjectileClass is NULL! Assign in BP!"));
		return;
	}

	// L_Weapon001 소켓 위치 가져오기
	FVector SpawnLocation = GetMesh()->GetSocketLocation(MagicSocketName);
	FRotator SpawnRotation = (PlayerCharacter->GetActorLocation() - SpawnLocation).Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	AActor* Projectile = GetWorld()->SpawnActor<AActor>(MagicProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (Projectile)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sg4Boss: Magic projectile fired!"));
	}
}

void ASg4BossCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bIsDead) return;

	if (CurrentAIState == ESg4BossState::AttackCooldown)
	{
		return;
	}

	bIsAttacking = false;
	CurrentAIState = ESg4BossState::AttackCooldown;

	GetWorldTimerManager().SetTimer(DecisionTimer, this, &ASg4BossCharacter::MakeDecision, AttackCooldownDuration, false);
}

float ASg4BossCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead) return 0.0f;

	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.0f)
	{
		CurrentHealth -= ActualDamage;

		if (CurrentHealth <= 0.0f)
		{
			bIsDead = true;
			CurrentAIState = ESg4BossState::Dead;

			GetWorldTimerManager().ClearTimer(DecisionTimer);
			GetCharacterMovement()->StopMovementImmediately();

			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SwordCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			DieUI();

			// BossRoomManager에 알림 (World 유효성 체크!)
			if (UWorld* World = GetWorld())
			{
				if (World->IsGameWorld() && !World->bIsTearingDown)
				{
					TArray<AActor*> FoundManagers;
					UGameplayStatics::GetAllActorsOfClass(World, ABossRoomManager::StaticClass(), FoundManagers);
					for (AActor* Manager : FoundManagers)
					{
						if (ABossRoomManager* BossManager = Cast<ABossRoomManager>(Manager))
						{
							BossManager->OnBossDefeated();
							UE_LOG(LogTemp, Warning, TEXT("Sg4Boss: Notified BossRoomManager"));
							break;
						}
					}
				}
			}

			if (DeathMontage)
			{
				PlayAnimMontage(DeathMontage);
				FOnMontageEnded MontageEndedDelegate;
				MontageEndedDelegate.BindUObject(this, &ASg4BossCharacter::OnDeathMontageEnded);
				GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndedDelegate, DeathMontage);
			}
			else
			{
				SetLifeSpan(3.0f);
			}
		}
	}
	return ActualDamage;
}

void ASg4BossCharacter::OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	Destroy();
}

void ASg4BossCharacter::ActivateSwordCollision()
{
	if (bIsDead) return;
	HitActors.Empty();

	if (SwordCollisionBox)
	{
		SwordCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		UE_LOG(LogTemp, Warning, TEXT("Sg4Boss: Sword collision ACTIVATED"));
	}
}

void ASg4BossCharacter::DeactivateSwordCollision()
{
	if (SwordCollisionBox)
	{
		SwordCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		UE_LOG(LogTemp, Warning, TEXT("Sg4Boss: Sword collision DEACTIVATED"));
	}
}

void ASg4BossCharacter::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsDead || OtherActor == this || HitActors.Contains(OtherActor)) return;

	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sg4Boss sword hit player! Damage: %.1f"), MeleeDamage);
		UGameplayStatics::ApplyDamage(Player, MeleeDamage, GetController(), this, UDamageType::StaticClass());
		HitActors.Add(Player);
	}
}