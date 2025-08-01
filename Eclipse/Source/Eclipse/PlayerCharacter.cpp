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

// 기본값 설정
APlayerCharacter::APlayerCharacter()
{
	// 매 프레임 Tick()을 호출하도록 이 캐릭터를 설정합니다. 필요하지 않은 경우 성능 향상을 위해 이 기능을 끌 수 있습니다.
	PrimaryActorTick.bCanEverTick = true;

	// 컨트롤러 회전을 사용하지 않습니다. 카메라가 컨트롤러 회전에 영향을 받지 않도록 합니다.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 캐릭터가 이동 방향으로 회전하도록 설정합니다.
	GetCharacterMovement()->bOrientRotationToMovement = true; 	
	// 회전 속도를 설정합니다.
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// 카메라 붐(카메라를 플레이어로부터 일정 거리에 유지)을 생성합니다.
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // 카메라가 플레이어를 따라가는 거리
	CameraBoom->bUsePawnControlRotation = true; // 컨트롤러를 기반으로 암을 회전합니다.

	// 따라가는 카메라를 생성합니다.
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // 카메라를 붐의 끝에 부착합니다.
	FollowCamera->bUsePawnControlRotation = false; // 카메라가 암을 기준으로 회전하지 않도록 합니다.

	// 도끼 컴포넌트를 생성합니다.
	AxeComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Axe"));
	AxeComponent->SetupAttachment(GetMesh(), FName("AxeSocket")); // 초기에는 등 소켓에 부착
	AxeComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AxeComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	AxeComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	AxeComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	bIsWeaponEquipped = false;
}

// 게임이 시작되거나 스폰될 때 호출됩니다.
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 기본 매핑 컨텍스트를 추가합니다.
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// 입력을 기능에 바인딩하기 위해 호출됩니다.
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Enhanced Input 컴포넌트를 설정합니다.
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 점프
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// 이동
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

		// 둘러보기
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

		// 무기 장착/해제
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &APlayerCharacter::ToggleWeapon);

		// 공격
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Attack);
	}
}

void APlayerCharacter::ToggleWeapon()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && (AnimInstance->Montage_IsPlaying(EquipMontage) || AnimInstance->Montage_IsPlaying(UnequipMontage)))
    {
        return;
    }

    if (bIsWeaponEquipped)
    {
        // 무기 해제
        if (UnequipMontage)
        {
            AnimInstance->Montage_Play(UnequipMontage);
            // 몽타주 길이를 가져와서 타이머 설정
            const float MontageLength = UnequipMontage->GetPlayLength();
            GetWorldTimerManager().SetTimer(UnequipTimerHandle, this, &APlayerCharacter::FinishUnequip, MontageLength, false);
        }
    }
    else
    {
        // 무기 장착
        if (EquipMontage)
        {
            AnimInstance->Montage_Play(EquipMontage);
            bIsWeaponEquipped = true;
            AxeComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("HandSocket"));
        }
    }
}

void APlayerCharacter::FinishUnequip()
{
    bIsWeaponEquipped = false;
    AxeComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("AxeSocket"));
    GetWorldTimerManager().ClearTimer(UnequipTimerHandle);
}

// 이동 입력을 처리하는 함수
void APlayerCharacter::Move(const FInputActionValue& Value)
{
	// 입력 벡터
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 컨트롤러의 회전값을 가져옵니다.
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 전방 및 오른쪽 방향을 계산합니다.
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 이동 입력을 추가합니다.
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

// 둘러보기 입력을 처리하는 함수
void APlayerCharacter::Look(const FInputActionValue& Value)
{
	// 입력 벡터
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 컨트롤러에 Yaw 및 Pitch 입력을 추가합니다.
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerCharacter::Attack(const FInputActionValue& Value)
{
	if (bIsWeaponEquipped)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && AttackMontage && !AnimInstance->Montage_IsPlaying(AttackMontage))
		{
			AnimInstance->Montage_Play(AttackMontage);
			// 공격 시작 시, 현재 프레임의 소켓 위치를 "이전 위치"로 저장합니다.
			PreviousBladeBaseLocation = AxeComponent->GetSocketLocation(TEXT("BladeBaseSocket"));
			PreviousBladeTipLocation = AxeComponent->GetSocketLocation(TEXT("BladeTipSocket"));
		}
	}
}

// 매 프레임 호출됩니다.
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (bIsWeaponEquipped && AnimInstance && AnimInstance->Montage_IsPlaying(AttackMontage))
	{
		// 현재 프레임의 도끼 날 소켓 위치를 가져옵니다.
		FVector CurrentBladeBaseLocation = AxeComponent->GetSocketLocation(TEXT("BladeBaseSocket"));
		FVector CurrentBladeTipLocation = AxeComponent->GetSocketLocation(TEXT("BladeTipSocket"));

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

		// 각 소켓 위치에 대한 충돌 결과를 저장할 변수
		FHitResult BaseHitResult, TipHitResult;

		// 1. 손잡이(Base) 부분 충돌 감지 (디버그 라인 표시)
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

		// 2. 도끼 날(Tip) 부분 충돌 감지 (디버그 라인 없음)
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

		// 두 충돌 중 하나라도 발생했다면
		if (bBaseHit || bTipHit)
		{
			// 실제 충돌이 일어난 결과를 사용합니다. (손잡이 부분이 우선)
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

		// 현재 프레임의 위치를 다음 프레임을 위해 "이전 위치"로 저장합니다.
		PreviousBladeBaseLocation = CurrentBladeBaseLocation;
		PreviousBladeTipLocation = CurrentBladeTipLocation;
	}
}