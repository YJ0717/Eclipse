#include "PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
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
	if (bIsAttacking)
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

	if (bIsRolling || GetCharacterMovement()->IsFalling())
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
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

void APlayerCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == DodgeMontage)
	{
		// 구르기 종료 후 0.1초 뒤에 상태를 리셋합니다.
		GetWorld()->GetTimerManager().SetTimer(DodgeEndTimerHandle, this, &APlayerCharacter::ResetDodgeState, 0.1f, false);
	}
	else
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
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageTaken = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (CurrentHealth <= 0.f) return DamageTaken; // 이미 죽었으면 추가 피해 처리 안함

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageTaken, 0.f, MaxHealth);

	if (CurrentHealth <= 0.f)
	{
		Die();
	}

	// UI 업데이트를 위한 델리게이트가 있다면 여기서 브로드캐스트 (나중에 추가)

	return DamageTaken;
}

void APlayerCharacter::Die()
{
	// 플레이어 사망 처리 로직 (예: 입력 비활성화, 사망 애니메이션 재생, 게임 오버 UI 표시 등)
	UE_LOG(LogTemp, Warning, TEXT("Player Died!"));
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 추가적인 사망 애니메이션, 게임 오버 UI 호출 등을 여기에 구현
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