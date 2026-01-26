// Fill out your copyright notice in the Description page of Project Settings.

#include "Sg3BossCharacter.h"
#include "Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"

ASg3BossCharacter::ASg3BossCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	CurrentAIState = ESg3BossState::Watching;
	bIsCharging = false;
	bIsAttacking = false;

	CurrentHealth = MaxHealth;

	RightWeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightWeaponCollisionBox"));
	RightWeaponCollisionBox->SetupAttachment(GetMesh(), FName("RightWeaponSocket"));
	RightWeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightWeaponCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void ASg3BossCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	bIsDead = false;

	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	// Mesh와 AnimInstance 체크
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp)
	{
		UE_LOG(LogTemp, Error, TEXT("Sg3Boss: Mesh Component is NULL!"));
	}
	else
	{
		USkeletalMesh* SkMesh = MeshComp->GetSkeletalMeshAsset();
		if (!SkMesh)
		{
			UE_LOG(LogTemp, Error, TEXT("Sg3Boss: Skeletal Mesh is NOT ASSIGNED!"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Sg3Boss: Skeletal Mesh OK: %s"), *SkMesh->GetName());
		}

		UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
		if (!AnimInst)
		{
			UE_LOG(LogTemp, Error, TEXT("Sg3Boss: AnimInstance is NULL! Anim Class not assigned or wrong mode!"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Sg3Boss: AnimInstance OK: %s"), *AnimInst->GetClass()->GetName());
		}
	}

	if (RightWeaponCollisionBox)
	{
		RightWeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ASg3BossCharacter::OnWeaponOverlap);
	}

	GetWorldTimerManager().SetTimer(DecisionTimer, this, &ASg3BossCharacter::MakeDecision, 0.5f, false);
}

void ASg3BossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDead) return;

	// World2Boss 완전 복사: bIsAttacking일 때 스킵
	if (bIsAttacking)
	{
		return;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, 
			FString::Printf(TEXT("Current State: %s"), *UEnum::GetValueAsString(CurrentAIState)));
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, 
			FString::Printf(TEXT("Boss HP: %f"), CurrentHealth));
	}

	if (PlayerCharacter)
	{
		FacePlayer(DeltaTime);
		ExecuteState(DeltaTime);
	}
}

void ASg3BossCharacter::FacePlayer(float DeltaTime)
{
	if (!PlayerCharacter || bIsDead) return;

	const FVector DirectionToPlayer = PlayerCharacter->GetActorLocation() - GetActorLocation();
	const FRotator TargetRotation = FRotator(0.0f, DirectionToPlayer.Rotation().Yaw, 0.0f);
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, RotationSpeed));
}

void ASg3BossCharacter::MakeDecision()
{
	// World2Boss 완전 복사
	if (bIsDead || CurrentAIState == ESg3BossState::Attacking || 
		CurrentAIState == ESg3BossState::BackingOff || bIsCharging || bIsAttacking) 
		return;

	float Distance = GetDistanceTo(PlayerCharacter);

	// World2Boss 로직 완전 복사!
	if (Distance <= AttackRange)
	{
		// 80% 확률로 공격, 15% 확률로 선회, 5% 확률로 후퇴
		int32 RandVal = FMath::RandRange(0, 100);
		if (RandVal < 80)
		{
			CurrentAIState = ESg3BossState::Attacking;
		}
		else if (RandVal < 95)
		{
			CurrentAIState = ESg3BossState::Circling;
			CirclingDirection = FMath::RandBool() ? 1.0f : -1.0f;
		}
		else
		{
			CurrentAIState = ESg3BossState::BackingOff;
		}
	}
	else if (Distance > RepositionDistance)
	{
		// 멀리 있으면 80% 확률로 접근
		if (FMath::RandRange(0, 100) < 80)
		{
			CurrentAIState = ESg3BossState::Approaching;
		}
		else
		{
			CurrentAIState = ESg3BossState::Watching;
		}
	}
	else
	{
		// 중간 거리: 다양한 행동
		int32 RandVal = FMath::RandRange(0, 100);
		if (RandVal < 50)
		{
			CurrentAIState = ESg3BossState::Circling;
			CirclingDirection = FMath::RandBool() ? 1.0f : -1.0f;
		}
		else if (RandVal < 70)
		{
			CurrentAIState = ESg3BossState::Watching;
		}
		else if (RandVal < 85)
		{
			CurrentAIState = ESg3BossState::Approaching;
		}
		else
		{
			CurrentAIState = ESg3BossState::Watching;
		}
	}

	GetWorldTimerManager().SetTimer(DecisionTimer, this, &ASg3BossCharacter::MakeDecision, DecisionInterval, false);
}

void ASg3BossCharacter::ExecuteState(float DeltaTime)
{
	// World2Boss 완전 복사!
	if (bIsDead)
	{
		GetCharacterMovement()->StopMovementImmediately();
		return;
	}

	if (CurrentAIState == ESg3BossState::Attacking || CurrentAIState == ESg3BossState::AttackCooldown)
	{
		GetCharacterMovement()->StopMovementImmediately();

		if (CurrentAIState == ESg3BossState::Attacking && !bIsAttacking)
		{
			PerformAttack();
		}
	}
	else
	{
		// World2Boss switch문 완전 복사!
		FVector MoveDirection = FVector::ZeroVector;

		switch (CurrentAIState)
		{
			case ESg3BossState::Watching:
				GetCharacterMovement()->MaxWalkSpeed = 0;
				break;

			case ESg3BossState::Circling:
				GetCharacterMovement()->MaxWalkSpeed = StrafeSpeed;
				MoveDirection = GetActorRightVector() * CirclingDirection;
				AddMovementInput(MoveDirection);
				break;

			case ESg3BossState::Approaching:
				if (!bIsCharging)
				{
					bIsCharging = true;
					GetCharacterMovement()->MaxWalkSpeed = ChargeSpeed;
					GetWorldTimerManager().SetTimer(ChargeTimer, this, &ASg3BossCharacter::OnChargeEnd, ChargeDuration, false);
				}
				MoveDirection = (PlayerCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
				AddMovementInput(MoveDirection);
				break;

			case ESg3BossState::BackingOff:
				GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
				MoveDirection = (GetActorLocation() - PlayerCharacter->GetActorLocation()).GetSafeNormal();
				AddMovementInput(MoveDirection);
				if (!GetWorldTimerManager().IsTimerActive(DecisionTimer))
				{
					GetWorldTimerManager().SetTimer(DecisionTimer, [this](){
						CurrentAIState = ESg3BossState::Watching;
						MakeDecision();
					}, RetreatDuration, false);
				}
				break;
		}
	}
}

void ASg3BossCharacter::PerformAttack()
{
	if (AttackMontages.Num() == 0 || bIsDead)
	{
		UE_LOG(LogTemp, Error, TEXT("PerformAttack FAILED: AttackMontages.Num=%d"), AttackMontages.Num());
		return;
	}

	bIsAttacking = true;

	// 랜덤으로 공격1 또는 공격2 선택
	int32 Index = FMath::RandRange(0, AttackMontages.Num() - 1);
	UAnimMontage* AttackToPlay = AttackMontages[Index];

	if (!AttackToPlay)
	{
		UE_LOG(LogTemp, Error, TEXT("PerformAttack: Selected montage is NULL at index %d"), Index);
		bIsAttacking = false;
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("========== PerformAttack: Playing '%s' =========="), *AttackToPlay->GetName());

	// World2Boss 방식으로 재생
	PlayAnimMontage(AttackToPlay);
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(this, &ASg3BossCharacter::OnAttackMontageEnded);
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, AttackToPlay);
		
		UE_LOG(LogTemp, Warning, TEXT("PerformAttack: EndDelegate set successfully"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PerformAttack: AnimInstance NULL, cannot set EndDelegate!"));
	}
}

void ASg3BossCharacter::OnChargeEnd()
{
	bIsCharging = false;
	CurrentAIState = ESg3BossState::Watching;
}

void ASg3BossCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Warning, TEXT("========== OnAttackMontageEnded CALLED! bInterrupted=%s =========="), 
		bInterrupted ? TEXT("true") : TEXT("false"));

	if (bIsDead) return;

	// 중복 호출 방지
	if (CurrentAIState == ESg3BossState::AttackCooldown)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnAttackMontageEnded: Already in cooldown, ignoring"));
		return;
	}

	bIsAttacking = false;
	CurrentAIState = ESg3BossState::AttackCooldown;

	UE_LOG(LogTemp, Warning, TEXT("OnAttackMontageEnded: Setting cooldown timer for %.1f seconds"), AttackCooldownDuration);

	GetWorldTimerManager().SetTimer(DecisionTimer, this, &ASg3BossCharacter::MakeDecision, AttackCooldownDuration, false);
}

float ASg3BossCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead) return 0.0f;

	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.0f)
	{
		CurrentHealth -= ActualDamage;

		if (CurrentHealth <= 0.0f)
		{
			bIsDead = true;
			CurrentAIState = ESg3BossState::Dead;

			GetWorldTimerManager().ClearTimer(DecisionTimer);
			GetCharacterMovement()->StopMovementImmediately();

			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			RightWeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			DieUI();

			if (DeathMontage)
			{
				PlayAnimMontage(DeathMontage);
				FOnMontageEnded MontageEndedDelegate;
				MontageEndedDelegate.BindUObject(this, &ASg3BossCharacter::OnDeathMontageEnded);
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

void ASg3BossCharacter::OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	Destroy();
}

void ASg3BossCharacter::ActivateWeaponCollision()
{
	if (bIsDead) return;
	HitActors.Empty();

	if (RightWeaponCollisionBox)
	{
		RightWeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void ASg3BossCharacter::DeactivateWeaponCollision()
{
	if (RightWeaponCollisionBox)
	{
		RightWeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ASg3BossCharacter::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsDead || OtherActor == this || HitActors.Contains(OtherActor)) return;

	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player)
	{
		UGameplayStatics::ApplyDamage(Player, AttackDamage, GetController(), this, UDamageType::StaticClass());
		HitActors.Add(Player);
	}
}
