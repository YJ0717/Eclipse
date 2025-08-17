#include "PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/StaticMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Sg1Monster1.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"
#include "RiposteDamageType.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 350.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 10.0f;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraRotationLagSpeed = 10.0f;
	CameraBoom->SocketOffset = FVector(0.f, 60.f, 70.f);
	CameraBoom->bDoCollisionTest = true;
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	AxeComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Axe"));
	AxeComponent->SetupAttachment(GetMesh(), FName("AxeSocket"));
	AxeComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	WeaponCollisionBox->SetupAttachment(AxeComponent);
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	WeaponCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	bIsWeaponEquipped = false;
	ComboCount = 0;
	bNextAttackRequested = false;
	bIsAttacking = false;

	// 구르기 상태 변수 초기화
	bIsRolling = false;

	// 패링 상태 변수 초기화
	RiposteTarget = nullptr;
	ParryStartTime = 0.1f;
	ParryEndTime = 0.4f;

	// 스태미너 변수 초기화
	MaxStamina = 100.f;
	CurrentStamina = MaxStamina;
	DodgeStaminaCost = 25.f;
	StaminaRegenRate = 15.f; // 초당 회복량
	bCanRegenStamina = true;

	// HP 변수 초기화
	MaxHealth = 100.f;
	CurrentHealth = MaxHealth;

	// 기본 달리기 속도를 600으로 설정합니다. (블루프린트에서 덮어쓸 수 있습니다)
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	OriginalMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	    WeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnWeaponOverlap);

    // 화면에 초기 스태미너 값 출력
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Initial Stamina: %.2f / %.2f"), CurrentStamina, MaxStamina));
    }
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &APlayerCharacter::ToggleWeapon);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Attack);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Dodge);
		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Started, this, &APlayerCharacter::StartWalking);
		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopWalking);
		EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Parry);
	}
}

void APlayerCharacter::ToggleWeapon()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (!AnimInstance || AnimInstance->IsAnyMontagePlaying()) return;
    UAnimMontage* MontageToPlay = bIsWeaponEquipped ? UnequipMontage : EquipMontage;
    if (MontageToPlay)
    {
        const float PlayRate = AnimInstance->Montage_Play(MontageToPlay);
        if (PlayRate > 0.f)
        {
            if (APlayerController* PC = Cast<APlayerController>(GetController()))
            {
                DisableInput(PC);
            }
            FOnMontageEnded MontageEndedDelegate;
            MontageEndedDelegate.BindUObject(this, &APlayerCharacter::OnMontageEnded);
            AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);
            if (bIsWeaponEquipped)
            {
                bIsWeaponEquipped = false;
                AxeComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("AxeSocket"));
            }
            else
            {
                bIsWeaponEquipped = true;
                AxeComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("HandSocket"));
            }
        }
    }
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	bool bIsParrying = AnimInstance && AnimInstance->Montage_IsPlaying(ParryMontage);

	if (bIsAttacking || bIsParrying)
	{
		return;
	}
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerCharacter::Attack()
{
	if (GetCharacterMovement()->IsFalling())
	{
		return;
	}

	// 리포스트(특수 공격) 확인
	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * 200.f; // 전방 200cm 확인
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	bool bHit = UKismetSystemLibrary::SphereTraceSingle(
		this,
		Start,
		End,
		100.f, // 1m 반경
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Pawn),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HitResult,
		true
	);

	if (bHit)
	{
		ASg1Monster1* Monster = Cast<ASg1Monster1>(HitResult.GetActor());
		if (Monster && Monster->IsStaggered())
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance && RiposteMontage && !AnimInstance->IsAnyMontagePlaying())
			{
				RiposteTarget = Monster;
				// 몬스터를 플레이어 앞으로 이동시키고 방향을 맞춤
				FVector TargetLocation = GetActorLocation() + GetActorForwardVector() * 120.f;
				FRotator TargetRotation = (GetActorLocation() - Monster->GetActorLocation()).Rotation();
				Monster->SetActorLocationAndRotation(TargetLocation, TargetRotation);
				
				AnimInstance->Montage_Play(RiposteMontage);
				return; // 리포스트 실행 시 일반 공격 로직은 실행하지 않음
			}
		}
	}


	if (bIsWeaponEquipped)
	{
		if (ComboCount == 0)
		{
			if (AttackMontages.IsValidIndex(0))
			{
				UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
				if (AnimInstance && !AnimInstance->IsAnyMontagePlaying())
				{
					bIsAttacking = true;
					ComboCount = 1;
					bNextAttackRequested = false;
					AnimInstance->Montage_Play(AttackMontages[0]);
				}
			}
		}
		else
		{
			bNextAttackRequested = true;
		}
	}
}

void APlayerCharacter::SaveAttack_Notify()
{
	if (bNextAttackRequested)
	{
		bNextAttackRequested = false;
		if (AttackMontages.IsValidIndex(ComboCount))
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(AttackMontages[ComboCount]);
				ComboCount++;
			}
		}
		else
		{
			ComboCount = 0;
		}
	}
}

void APlayerCharacter::ResetCombo_Notify()
{
	ComboCount = 0;
	bNextAttackRequested = false;
	bIsAttacking = false;
}

void APlayerCharacter::StartWalking(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = OriginalMaxWalkSpeed / 2.0f;
}

void APlayerCharacter::StopWalking(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = OriginalMaxWalkSpeed;
}

void APlayerCharacter::ResetDodgeState()
{
	bIsRolling = false;
	GetCapsuleComponent()->SetCapsuleHalfHeight(OriginalCapsuleHalfHeight);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -OriginalCapsuleHalfHeight));

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		EnableInput(PC);
	}

	// 구르기가 끝나면 스태미너 회복 시작
	bCanRegenStamina = true;
}

void APlayerCharacter::Dodge(const FInputActionValue& Value)
{
	// 스태미너가 부족하면 구르기 불가
	if (CurrentStamina < DodgeStaminaCost)
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	bool bIsParrying = AnimInstance && AnimInstance->Montage_IsPlaying(ParryMontage);

	if (bIsRolling || GetCharacterMovement()->IsFalling() || bIsAttacking || bIsParrying)
	{
		return;
	}

	if (AnimInstance && DodgeMontage && !AnimInstance->IsAnyMontagePlaying())
	{
		// 스태미너 소모 및 회복 중지
		CurrentStamina -= DodgeStaminaCost;
		bCanRegenStamina = false;

		bIsRolling = true;
		
		OriginalCapsuleHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

		FVector LastInputDirection = GetCharacterMovement()->GetLastInputVector().GetSafeNormal();
		if (LastInputDirection.IsNearlyZero())
		{
			LastInputDirection = GetActorForwardVector();
		}
		const FRotator DodgeRotation = LastInputDirection.Rotation();
		SetActorRotation(DodgeRotation);

		const float PlayRate = AnimInstance->Montage_Play(DodgeMontage);
		if (PlayRate > 0.f)
		{
			GetCapsuleComponent()->SetCapsuleHalfHeight(OriginalCapsuleHalfHeight / 2.0f);
			GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -OriginalCapsuleHalfHeight / 2.0f));

			if (APlayerController* PC = Cast<APlayerController>(GetController()))
			{
				DisableInput(PC);
			}

			FOnMontageEnded MontageEndedDelegate;
			MontageEndedDelegate.BindUObject(this, &APlayerCharacter::OnMontageEnded);
			AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, DodgeMontage);
		}
		else
		{
			bIsRolling = false;
		}
	}
}

void APlayerCharacter::Parry()
{
	if (!bIsWeaponEquipped) return; // 무기를 들었을 때만 패링 가능

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ParryMontage && !AnimInstance->IsAnyMontagePlaying())
	{
		AnimInstance->Montage_Play(ParryMontage);
	}
}

void APlayerCharacter::ApplyRiposteDamage()
{
	if (RiposteTarget)
	{
		UGameplayStatics::ApplyDamage(RiposteTarget, RiposteDamage, GetController(), this, URiposteDamageType::StaticClass());
		RiposteTarget = nullptr; // 타겟 초기화
	}
}


void APlayerCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == DodgeMontage)
	{
		// 구르기 종료 후 0.1초 뒤에 상태를 리셋합니다.
		GetWorld()->GetTimerManager().SetTimer(DodgeEndTimerHandle, this, &APlayerCharacter::ResetDodgeState, 0.1f, false);
	}
	// HitMontage는 OnHitAnimationEnded에서 처리되므로 여기서는 일반적인 입력 활성화 로직을 제거합니다.
	// 다른 몽타주가 끝났을 때만 입력 활성화
	else if (Montage != HitMontage && Montage != DeathMontage && Montage != ParryMontage) // ParryMontage도 추가
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			EnableInput(PC);
		}
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 스태미너 회복 로직
	if (bCanRegenStamina && CurrentStamina < MaxStamina)
	{
		CurrentStamina = FMath::Min(MaxStamina, CurrentStamina + StaminaRegenRate * DeltaTime);
	}

    // 화면에 현재 스태미너 값 출력
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow, FString::Printf(TEXT("Current Stamina: %.2f"), CurrentStamina));
    }

	// 패링 가능 상태일 때 디버그 스피어 표시
	if (IsParryWindowActive())
	{
		FVector Center = GetActorLocation() + GetActorForwardVector() * 75.f; // 75cm 앞으로
		float Radius = 75.f;
		DrawDebugSphere(GetWorld(), Center, Radius, 12, FColor::Cyan, false, 0.f, 0, 1.f);
	}
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// 패링 성공 로직
	if (IsParryWindowActive() && DamageCauser)
	{
		ASg1Monster1* Monster = Cast<ASg1Monster1>(DamageCauser);
		if (Monster)
		{
			Monster->GetParried();
			// 패링 성공 효과음 또는 파티클 재생 (선택 사항)
			// UGameplayStatics::PlaySoundAtLocation(this, ParrySuccessSound, GetActorLocation());
			return 0.f; // 데미지 무효화
		}
	}


	float DamageTaken = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (CurrentHealth <= 0.f) return DamageTaken; // 이미 죽었으면 추가 피해 처리 안함

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageTaken, 0.f, MaxHealth);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (CurrentHealth <= 0.f)
	{
		Die();
	}
	else // 아직 살아있다면 피격 애니메이션 재생
	{
		// 피격 시 공격 및 구르기 상태를 초기화하여 굳는 버그를 방지합니다.
		bIsAttacking = false;
		bIsRolling = false;
		ComboCount = 0;
		bNextAttackRequested = false;

		// 현재 재생 중인 모든 몽타주를 중지시킵니다.
		if (AnimInstance)
		{
			AnimInstance->StopAllMontages(0.2f);
		}

		if (AnimInstance && HitMontage)
		{
			const float PlayRate = AnimInstance->Montage_Play(HitMontage);
			if (PlayRate > 0.f)
			{
				if (APlayerController* PC = Cast<APlayerController>(GetController()))
				{
					DisableInput(PC); // 피격 중 입력 비활성화
				}
				FOnMontageEnded MontageEndedDelegate;
				MontageEndedDelegate.BindUObject(this, &APlayerCharacter::OnHitAnimationEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, HitMontage);
			}
		}
	}

	// UI 업데이트를 위한 델리게이트가 있다면 여기서 브로드캐스트 (나중에 추가)

	return DamageTaken;
}

void APlayerCharacter::Die()
{
	// 플레이어 사망 처리 로직
	UE_LOG(LogTemp, Warning, TEXT("Player Died!"));

	// 입력 영구 비활성화
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}

	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(this, &APlayerCharacter::OnDeathAnimationEnded);
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, DeathMontage);
	}
	else
	{
		// 사망 애니메이션이 없으면 바로 액터 숨김/파괴
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		SetLifeSpan(0.1f); // 짧은 수명 설정
	}
}

void APlayerCharacter::OnDeathAnimationEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 사망 애니메이션 종료 후 처리 (예: 액터 숨김, 게임 오버 UI 호출 등)
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetLifeSpan(0.1f); // 짧은 수명 설정
	// 게임 오버 UI 호출 등을 여기에 구현
}

void APlayerCharacter::OnHitAnimationEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 피격 애니메이션 종료 후 입력 재활성화
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		EnableInput(PC);
	}
}

void APlayerCharacter::StartAttackCollision()
{
	HitActors.Empty();
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void APlayerCharacter::StopAttackCollision()
{
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APlayerCharacter::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this) return;
	if (HitActors.Contains(OtherActor))
	{
		return;
	}
	ASg1Monster1* Monster = Cast<ASg1Monster1>(OtherActor);
	if (Monster)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon Hit Monster: %s"), *Monster->GetName());
		UGameplayStatics::ApplyDamage(Monster, AttackDamage, GetController(), this, UDamageType::StaticClass());
		HitActors.Add(OtherActor);
		if (ImpactEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, SweepResult.ImpactPoint, SweepResult.ImpactNormal.Rotation());
		}
	}
}

bool APlayerCharacter::IsParryWindowActive() const
{
    if (!GetMesh() || !ParryMontage)
    {
        return false;
    }

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && AnimInstance->Montage_IsPlaying(ParryMontage))
    {
        const float CurrentPosition = AnimInstance->Montage_GetPosition(ParryMontage);
        return CurrentPosition >= ParryStartTime && CurrentPosition <= ParryEndTime;
    }
    return false;
}