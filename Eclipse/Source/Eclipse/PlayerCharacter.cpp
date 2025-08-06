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
#include "Sg1Monster1.h"
#include "Components/BoxComponent.h" // <--- 추가

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
	AxeComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); // <--- 충돌 비활성화

	// 무기 충돌 박스 생성 및 설정
	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	WeaponCollisionBox->SetupAttachment(AxeComponent);
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	WeaponCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	bIsWeaponEquipped = false;
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

	// Overlap 이벤트에 함수 바인딩
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
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Attack); // <--- 변경
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

void APlayerCharacter::Attack() // <--- 변경
{
	if (GetCharacterMovement()->IsFalling())
	{
		return;
	}

	if (bIsWeaponEquipped)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && AttackMontage && !AnimInstance->IsAnyMontagePlaying())
		{
            const float PlayRate = AnimInstance->Montage_Play(AttackMontage);
            if (PlayRate > 0.f)
            {
                if (APlayerController* PC = Cast<APlayerController>(GetController()))
                {
                    DisableInput(PC);
                }

                FOnMontageEnded MontageEndedDelegate;
                MontageEndedDelegate.BindUObject(this, &APlayerCharacter::OnMontageEnded);
                AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, AttackMontage);
            }
		}
	}
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
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DodgeMontage && !AnimInstance->IsAnyMontagePlaying())
	{
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
		}
	}

	// 기존의 Tick 기반 충돌 검사 로직을 모두 삭제했습니다.
}

// --- 아래 함수들을 새로 추가 ---

void APlayerCharacter::StartAttackCollision()
{
	HitActors.Empty(); // 맞은 액터 목록 초기화
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // 충돌 활성화
}

void APlayerCharacter::StopAttackCollision()
{
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 충돌 비활성화
}

void APlayerCharacter::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this) return; // 자기 자신은 무시

	// 이미 공격한 액터인지 확인
	if (HitActors.Contains(OtherActor))
	{
		return;
	}

	// 몬스터인지 확인 (클래스 캐스팅 또는 태그 사용)
	ASg1Monster1* Monster = Cast<ASg1Monster1>(OtherActor);
	if (Monster)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon Hit Monster: %s"), *Monster->GetName());

		// 데미지 적용
		UGameplayStatics::ApplyDamage(Monster, AttackDamage, GetController(), this, UDamageType::StaticClass());

		// 맞은 액터 목록에 추가
		HitActors.Add(OtherActor);

		// 파티클 효과 생성
		if (ImpactEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, SweepResult.ImpactPoint, SweepResult.ImpactNormal.Rotation());
		}
	}
}