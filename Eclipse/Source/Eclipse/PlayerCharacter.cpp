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

void APlayerCharacter::Dodge(const FInputActionValue& Value)
{
	// 구르기가 이미 진행 중인 경우, 새로운 구르기를 시작하지 않음
	if (bIsRolling)
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DodgeMontage && !AnimInstance->IsAnyMontagePlaying())
	{
		bIsRolling = true; // 구르기 시작
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
			OriginalCapsuleHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
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
	}
}

void APlayerCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == DodgeMontage)
	{
		bIsDodgeEnding = true;
		DodgeEndTimer = 0.0f;
	}
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		EnableInput(PC);
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsDodgeEnding)
	{
		DodgeEndTimer += DeltaTime;
		const float DodgeEndDuration = 0.2f;
		const float NewHalfHeight = FMath::Lerp(GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), OriginalCapsuleHalfHeight, DodgeEndTimer / DodgeEndDuration);
		GetCapsuleComponent()->SetCapsuleHalfHeight(NewHalfHeight);
		const FVector NewMeshLocation = FMath::Lerp(GetMesh()->GetRelativeLocation(), FVector(0.f, 0.f, -96.f), DodgeEndTimer / DodgeEndDuration);
		GetMesh()->SetRelativeLocation(NewMeshLocation);
		if (DodgeEndTimer >= DodgeEndDuration)
		{
						bIsDodgeEnding = false;
			GetCapsuleComponent()->SetCapsuleHalfHeight(OriginalCapsuleHalfHeight);
			GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -96.f));

			// 구르기 상태를 완전히 종료하고 다음 입력을 받을 수 있도록 함
			bIsRolling = false;
		}
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