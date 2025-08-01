// Fill out your copyright notice in the Description page of Project Settings.

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
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	AxeComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Axe"));
	AxeComponent->SetupAttachment(GetMesh(), FName("AxeSocket"));
	AxeComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AxeComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	AxeComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	AxeComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	bIsWeaponEquipped = false;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
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
	}
}

void APlayerCharacter::ToggleWeapon()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (!AnimInstance || AnimInstance->IsAnyMontagePlaying()) return;

    UAnimMontage* MontageToPlay = bIsWeaponEquipped ? UnequipMontage : EquipMontage;
    if (MontageToPlay)
    {
        float PlayRate = AnimInstance->Montage_Play(MontageToPlay);
        if (PlayRate > 0.f)
        {
            GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

            FOnMontageEnded MontageEndedDelegate;
            MontageEndedDelegate.BindUObject(this, &APlayerCharacter::OnMontageEnded);
            AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

            // 몽타주 재생이 끝난 후 상태를 변경합니다.
            if (bIsWeaponEquipped)
            {
                // Unequip
                bIsWeaponEquipped = false;
                AxeComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("AxeSocket"));
            }
            else
            {
                // Equip
                bIsWeaponEquipped = true;
                AxeComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("HandSocket"));
            }
        }
    }
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

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
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerCharacter::Attack(const FInputActionValue& Value)
{
	if (bIsWeaponEquipped)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && AttackMontage && !AnimInstance->IsAnyMontagePlaying())
		{
            float PlayRate = AnimInstance->Montage_Play(AttackMontage);
            if (PlayRate > 0.f)
            {
                GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

                FOnMontageEnded MontageEndedDelegate;
                MontageEndedDelegate.BindUObject(this, &APlayerCharacter::OnMontageEnded);
                AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, AttackMontage);

                PreviousBladeBaseLocation = AxeComponent->GetSocketLocation(TEXT("BladeBaseSocket"));
                PreviousBladeTipLocation = AxeComponent->GetSocketLocation(TEXT("BladeTipSocket"));
            }
		}
	}
}

void APlayerCharacter::Dodge(const FInputActionValue& Value)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DodgeMontage && !AnimInstance->IsAnyMontagePlaying())
	{
        FVector LastInputDirection = GetCharacterMovement()->GetLastInputVector().GetSafeNormal();
        if (LastInputDirection.IsNearlyZero())
        {
            LastInputDirection = GetActorForwardVector();
        }

        FRotator DodgeRotation = LastInputDirection.Rotation();
        SetActorRotation(DodgeRotation);

        float PlayRate = AnimInstance->Montage_Play(DodgeMontage);
        if (PlayRate > 0.f)
        {
            GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

            FOnMontageEnded MontageEndedDelegate;
            MontageEndedDelegate.BindUObject(this, &APlayerCharacter::OnMontageEnded);
            AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, DodgeMontage);
        }
	}
}

void APlayerCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 몽타주가 종료되면(중단 포함), 캐릭터의 움직임 모드를 다시 걷기 상태로 되돌립니다.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (bIsWeaponEquipped && AnimInstance && AnimInstance->Montage_IsPlaying(AttackMontage))
	{
		FVector CurrentBladeBaseLocation = AxeComponent->GetSocketLocation(TEXT("BladeBaseSocket"));
		FVector CurrentBladeTipLocation = AxeComponent->GetSocketLocation(TEXT("BladeTipSocket"));

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

		FHitResult BaseHitResult, TipHitResult;

		bool bBaseHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
			GetWorld(),
			PreviousBladeBaseLocation,
			CurrentBladeBaseLocation,
			10.0f,
			ObjectTypes,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForDuration,
			BaseHitResult,
			true
		);

		bool bTipHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
			GetWorld(),
			PreviousBladeTipLocation,
			CurrentBladeTipLocation,
			10.0f,
			ObjectTypes,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			TipHitResult,
			true
		);

		if (bBaseHit || bTipHit)
		{
			const FHitResult& HitResult = bBaseHit ? BaseHitResult : TipHitResult;
			AActor* HitActor = HitResult.GetActor();

			if (HitActor)
			{
				UGameplayStatics::ApplyDamage(HitActor, 10.f, GetController(), this, UDamageType::StaticClass());
				AnimInstance->Montage_Stop(0.1f, AttackMontage);

				if (ImpactEffect)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
				}
			}
		}

		PreviousBladeBaseLocation = CurrentBladeBaseLocation;
		PreviousBladeTipLocation = CurrentBladeTipLocation;
	}
}
