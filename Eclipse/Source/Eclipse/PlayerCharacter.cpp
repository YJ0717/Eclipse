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
#include "Sg1Monster2.h"
#include "Sg1BossCharacter.h"
#include "World2Boss/World2AIBossCharacter.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"
#include "RiposteDamageType.h"
#include "EclipseGameInstance.h" // 게임 인스턴스 헤더
#include "EclipseSaveGame.h" // 세이브 시스템 헤더
#include "SaveGameData.h" // 세이브 데이터 헤더


// DestructibleWall 헤더 추가
#include "Destructible/DestructibleWall.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Destructible/PillarDestructible.h"

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
	WeaponCollisionBox->SetNotifyRigidBodyCollision(true);
	// 중요: 게임 시작 시 무기 콜리전이 확실하게 꺼져 있도록 명시적으로 설정합니다.
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);



	// 수정: 기본적으로 모든 것에 대해 무시(Ignore)로 설정
	WeaponCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	// 수정: Pawn에 대해서는 Overlap으로 설정 (기존 몬스터 타격 로직 유지)
	WeaponCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	// 수정: WorldDynamic에 대해서는 Block으로 설정 (파괴 가능한 벽과의 충돌 감지용)
	WeaponCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	//WeaponCollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);// 보스 피격 ;;
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
	
	//에스트관련 초기화
	MaxEst = 5.f;
	CurrentEst = MaxEst;
	
	// 기본 달리기 속도를 600으로 설정합니다. (블루프린트에서 덮어쓸 수 있습니다)
	GetCharacterMovement()->MaxWalkSpeed = 600.f;

	ParryCameraTimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("ParryCameraTimeline"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (ParryCameraCurve)
	{
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("UpdateParryCamera"));
		ParryCameraTimelineComp->AddInterpFloat(ParryCameraCurve, ProgressFunction);
	}

	OriginalMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	// 기존 Overlap 이벤트 바인딩
	WeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnWeaponOverlap);
	// 새로 추가: Hit 이벤트 바인딩
	WeaponCollisionBox->OnComponentHit.AddDynamic(this, &APlayerCharacter::OnWeaponHit);

    // 화면에 초기 스태미너 값 출력
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Initial Stamina: %.2f / %.2f"), CurrentStamina, MaxStamina));
        }
    
        // --- 게임 로드 로직 추가 ---
        UEclipseGameInstance* GameInstance = Cast<UEclipseGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
        if (GameInstance)
        {
            // "Continue" 버튼으로 게임을 로드하는 경우
            if (GameInstance->bShouldLoadGame)
            {
                LoadPlayerState();
                GameInstance->bShouldLoadGame = false; // 한 번 로드 후 플래그 리셋
            }
            // 포탈을 통해 레벨을 이동한 경우
            else if (GameInstance->PlayerHealthOnTravel > 0.f)
            {
                CurrentHealth = GameInstance->PlayerHealthOnTravel;
                GameInstance->PlayerHealthOnTravel = 0.f; // 한 번 적용 후 리셋
            }
        }
}
// 커스텀 채널 정의 (DefaultEngine.ini 기반)
#define ECC_OutDestruction ECollisionChannel::ECC_GameTraceChannel2
#define ECC_InnerDestruction ECollisionChannel::ECC_GameTraceChannel3

void APlayerCharacter::OnWeaponHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // 부딪힌 액터가 DestructibleWall인지 확인합니다.
    ADestructibleWall* Wall = Cast<ADestructibleWall>(OtherActor);
    if (Wall)
    {
        Wall->ApplyDamageAtLocation(Hit.Location, 50.f, 100.f);
    }

    // 부딪힌 액터가 PillarDestructible인지 확인합니다.
    APillarDestructible* Pillar = Cast<APillarDestructible>(OtherActor);
    if (Pillar)
    {
        // 맞은 컴포넌트의 오브젝트 타입으로 겉과 속을 구별
        ECollisionChannel HitObjectType = OtherComp->GetCollisionObjectType();
        if (HitObjectType == ECC_OutDestruction)
        {
            Pillar->ApplyOuterShellDamage(Hit.ImpactPoint);
        }
        else if (HitObjectType == ECC_InnerDestruction)
        {
            Pillar->ApplyInnerCoreDamage(Hit.ImpactPoint);
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
		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Started, this, &APlayerCharacter::StartWalking);
		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopWalking);
		EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Parry);
		EnhancedInputComponent->BindAction(SkillAction, ETriggerEvent::Started, this, &APlayerCharacter::Skill);
		EnhancedInputComponent->BindAction(HealAction, ETriggerEvent::Started, this, &APlayerCharacter::Heal);

		// 세이브 입력 바인딩
		EnhancedInputComponent->BindAction(SaveAction, ETriggerEvent::Started, this, &APlayerCharacter::SavePlayerState);
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

	if (bIsAttacking || bIsParrying || bIsUsingSkill)
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
					// 현재 속도 가져오기
					float Speed = GetCharacterMovement()->Velocity.Size();
					if (Speed >= 600.f) bIsRunningAttack = true;
					
					// 속도에 따라 재생할 몽타주 선택
					UAnimMontage* MontageToPlay = (Speed >= 600.f) ? RunAttackMontages[0] : AttackMontages[0];

					AnimInstance->Montage_Play(MontageToPlay);
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
		TArray<UAnimMontage*>& SelectedMontages = bIsRunningAttack ? RunAttackMontages : AttackMontages;

		if (SelectedMontages.IsValidIndex(ComboCount))
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(SelectedMontages[ComboCount]);
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
	bIsRunningAttack = false;
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

	if (GetCharacterMovement()->IsFalling()) return; // 공중 패링X

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ParryMontage && !AnimInstance->IsAnyMontagePlaying())
	{
		AnimInstance->Montage_Play(ParryMontage);
		ParryCameraTimelineComp->PlayFromStart(); // 카메라 줌 타임라인 재생
	}
}

void APlayerCharacter::UpdateParryCamera(float Value)
{
	if (CameraBoom)
	{
		CameraBoom->TargetArmLength = Value;
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
	else if (Montage != HitMontage && Montage != DeathMontage && Montage != ParryMontage && Montage != HealMontage) // ParryMontage도 추가
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

	CameraBoom->TargetArmLength = FMath::Clamp(
		CameraBoom->TargetArmLength,
		MinTargetArmLength,
		DefaultTargetArmLength
	);

	// 2) 보스와 거리 체크
	AActor* Boss = UGameplayStatics::GetActorOfClass(GetWorld(), ASg1BossCharacter::StaticClass());
	if (Boss)
	{
		float DistanceToBoss = FVector::Dist(GetActorLocation(), Boss->GetActorLocation());

		if (DistanceToBoss < BossNearDistance)
		{
			// 보스 가까우면 카메라 위로 이동
			CameraBoom->SocketOffset = FMath::VInterpTo(
				CameraBoom->SocketOffset,
				BossSocketOffset,
				DeltaTime,
				5.0f // 부드러운 전환 속도
			);
		}
		else
		{
			// 보스 멀어지면 원래대로
			CameraBoom->SocketOffset = FMath::VInterpTo(
				CameraBoom->SocketOffset,
				DefaultSocketOffset,
				DeltaTime,
				5.0f
			);
		}
	}
	//3 구르기 공격  보정
	// 점프/낙하 중엔 적용 X
	if (GetCharacterMovement()->IsFalling())
		return;

	// 공격·구르기 중에만 적용
	if (bIsAttacking || bIsRolling)
	{
		FHitResult FloorHit;
		FVector Start = GetActorLocation();
		FVector End = Start - FVector(0, 0, 150.f);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(FloorHit, Start, End, ECC_Visibility, Params))
		{
			float FloorDist = (Start.Z - FloorHit.ImpactPoint.Z);

			// 바닥과 거리 5cm 이상이면 붙이기
			if (FloorDist > 5.f)
			{
				FVector TargetLoc = GetActorLocation();
				TargetLoc.Z -= FMath::Clamp(FloorDist - 5.f, 0.f, 10.f); // 부드럽게 보정
				SetActorLocation(TargetLoc, true);
			}
		}
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
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
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

	ASg1Monster2* Monster2 = Cast<ASg1Monster2>(OtherActor);
	if (Monster2 && !HitActors.Contains(Monster2))
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon Hit Monster2: %s"), *Monster2->GetName());
		UGameplayStatics::ApplyDamage(Monster2, AttackDamage, GetController(), this, UDamageType::StaticClass());
		HitActors.Add(Monster2);

		if (ImpactEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, SweepResult.ImpactPoint, SweepResult.ImpactNormal.Rotation());
		}
	}

	ASg1BossCharacter* Boss = Cast<ASg1BossCharacter>(OtherActor);
	if (Boss)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PlayerWeapon] Hit Boss %s Comp=%s"),
			*Boss->GetName(), OtherComp ? *OtherComp->GetName() : TEXT("NULL"));

		UGameplayStatics::ApplyPointDamage(
			Boss,
			AttackDamage,
			GetActorForwardVector(),
			SweepResult,    
			GetController(),
			this,
			UDamageType::StaticClass()
		);

		HitActors.Add(OtherActor);

		if (ImpactEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ImpactEffect,
				SweepResult.ImpactPoint,
				SweepResult.ImpactNormal.Rotation()
			);
		}
	}

	// 새로 추가: World2Boss와의 충돌 처리
	AWorld2AIBossCharacter* World2Boss = Cast<AWorld2AIBossCharacter>(OtherActor);
	if (World2Boss && !HitActors.Contains(World2Boss))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player's weapon hit World2Boss: %s"), *World2Boss->GetName());
		UGameplayStatics::ApplyDamage(World2Boss, AttackDamage, GetController(), this, UDamageType::StaticClass());
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

void APlayerCharacter::Skill(const FInputActionValue& Value)
{
	if (bIsWeaponEquipped && SkillMontages.IsValidIndex(SkillAttack))
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && !AnimInstance->IsAnyMontagePlaying())
		{
			
			bIsUsingSkill = true;

			
			GetCharacterMovement()->DisableMovement();

			
			AnimInstance->Montage_Play(SkillMontages[SkillAttack]);

			
			FOnMontageEnded MontageEndedDelegate;
			MontageEndedDelegate.BindUObject(this, &APlayerCharacter::OnSkillMontageEnded);
			AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, SkillMontages[SkillAttack]);
		}
	}
}
void APlayerCharacter::OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsUsingSkill = false;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void APlayerCharacter::Heal(const FInputActionValue& Value)
{
	if (bIsHealing || CurrentEst <= 0)
		return;

	float HealAmount = 50.f;
	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.f, MaxHealth);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HealMontage)
	{
		bIsHealing = true;
		AnimInstance->Montage_Play(HealMontage);

		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(this, &APlayerCharacter::OnHealMontageEnded);
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, HealMontage);
	}

	CurrentEst -= 1;
}

void APlayerCharacter::OnHealMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == HealMontage)
	{
		bIsHealing = false;
	}
}

void APlayerCharacter::InitializeTraits()//현재 쓰지는 않지만 나중에 죽고 정렬시킬때 쓸수도있어서 남겨둠
{
	AllTraits.Empty();

	
}

TArray<FTraitData> APlayerCharacter::GetRandomTraits(int32 Count)// 현재 쓰지는 않지만 특성 여러개 되면 사용예정
{
	TArray<FTraitData> Result;
	TArray<int32> UsedIndices;

	while (Result.Num() < Count && Result.Num() < AllTraits.Num())
	{
		int32 RandIndex = FMath::RandRange(0, AllTraits.Num() - 1);
		if (!UsedIndices.Contains(RandIndex))
		{
			Result.Add(AllTraits[RandIndex]);
			UsedIndices.Add(RandIndex);
		}
	}

	return Result;
}

void APlayerCharacter::ApplyTrait(const FTraitData& Trait)
{
	switch (Trait.TraitType)
	{
	case ETraitType::AttackDamageUp:
		AttackDamage += Trait.Value;
		break;

	case ETraitType::SkillBuff:
		SkillDamageRate *= Trait.Value;
		break;

	case ETraitType::SkillChange:
		SkillAttack = Trait.Value;
		break;

	default:
		break;
	}
}
void APlayerCharacter::SavePlayerState()
{
    // "MySaveSlot"이라는 이름으로 현재 플레이어의 위치와 HP를 저장합니다.
    UEclipseSaveGame::SaveGame("MySaveSlot", 0, TEXT("Player1"), GetActorLocation(), CurrentHealth, UGameplayStatics::GetCurrentLevelName(GetWorld()));

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Game Saved! Location: %s, HP: %.1f"), *GetActorLocation().ToString(), CurrentHealth));
    }
}

void APlayerCharacter::LoadPlayerState()
{
    // "MySaveSlot"에 저장된 게임 데이터를 불러옵니다.
    USaveGameData* LoadedGame = UEclipseSaveGame::LoadGame("MySaveSlot", 0);
    if (LoadedGame)
    {
        // 불러온 위치와 HP로 플레이어를 설정합니다.
        SetActorLocation(LoadedGame->PlayerLocation);
        CurrentHealth = LoadedGame->PlayerHealth;

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Game Loaded! Player moved to: %s, HP set to: %.1f"), *LoadedGame->PlayerLocation.ToString(), CurrentHealth));
        }
    }
    else
    { 
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Save Game found!"));
        }
    }
}