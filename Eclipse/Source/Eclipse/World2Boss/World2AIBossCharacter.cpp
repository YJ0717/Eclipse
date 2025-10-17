#include "World2Boss/World2AIBossCharacter.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"

AWorld2AIBossCharacter::AWorld2AIBossCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = false;

    CurrentAIState = EBossAIState::Watching;
    bIsCharging = false;
    bIsSideDashing = false;
    bCanDoNextCombo = false;
    bIsComboAttacking = false;
    CurrentComboIndex = 0;

    // 오른손 무기 충돌 박스를 생성하고 오른손 무기 소켓에 붙입니다.
    RightWeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightWeaponCollisionBox"));
    RightWeaponCollisionBox->SetupAttachment(GetMesh(), FName("RightWeaponSocket"));
    RightWeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RightWeaponCollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    RightWeaponCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    RightWeaponCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

    // 왼손 무기 충돌 박스를 생성하고 왼손 무기 소켓에 붙입니다.
    LeftWeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWeaponCollisionBox"));
    LeftWeaponCollisionBox->SetupAttachment(GetMesh(), FName("LeftWeaponSocket"));
    LeftWeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    LeftWeaponCollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    LeftWeaponCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    LeftWeaponCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AWorld2AIBossCharacter::BeginPlay()
{
    Super::BeginPlay();

    PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    // 양손 무기 충돌 박스의 오버랩 이벤트에 함수를 바인딩합니다.
    if (RightWeaponCollisionBox)
    {
        RightWeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AWorld2AIBossCharacter::OnWeaponOverlap);
    }
    if (LeftWeaponCollisionBox)
    {
        LeftWeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AWorld2AIBossCharacter::OnWeaponOverlap);
    }

    GetWorldTimerManager().SetTimer(DecisionTimer, this, &AWorld2AIBossCharacter::MakeDecision, 0.5f, false);
}

void AWorld2AIBossCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 공격 중에는 회전 및 이동 등 모든 AI 로직을 정지시켜 제자리에 고정합니다.
    if (bIsComboAttacking)
    {
        return;
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("Current State: %s"), *UEnum::GetValueAsString(CurrentAIState)));
    }

    if (PlayerCharacter)
    {
        FacePlayer(DeltaTime);
        ExecuteState(DeltaTime);
    }
}

void AWorld2AIBossCharacter::FacePlayer(float DeltaTime)
{
    if (!PlayerCharacter) return;

    const FVector DirectionToPlayer = PlayerCharacter->GetActorLocation() - GetActorLocation();
    const FRotator TargetRotation = FRotator(0.0f, DirectionToPlayer.Rotation().Yaw, 0.0f);
    SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, RotationSpeed));
}

void AWorld2AIBossCharacter::MakeDecision()
{
    if (CurrentAIState == EBossAIState::Attacking || CurrentAIState == EBossAIState::BackingOff || bIsCharging || bIsSideDashing || bIsComboAttacking) return;

    float Distance = GetDistanceTo(PlayerCharacter);

    // 1. 플레이어가 공격 범위 안에 있는가?
    if (Distance <= AttackRange)
    {
        // 80% 확률로 공격, 15% 확률로 선회, 5% 확률로만 거리를 벌립니다.
        int32 RandVal = FMath::RandRange(0, 100);
        if (RandVal < 80)
        {
            CurrentAIState = EBossAIState::Attacking;
        }
        else if (RandVal < 95)
        {
            CurrentAIState = EBossAIState::Circling;
            CirclingDirection = FMath::RandBool() ? 1.0f : -1.0f;
        }
        else
        {
            CurrentAIState = EBossAIState::BackingOff;
        }
    }
    else if (Distance > RepositionDistance)
    {
        if (FMath::RandRange(0, 100) < 80)
        {
            CurrentAIState = EBossAIState::Approaching;
        }
        else
        {
            CurrentAIState = EBossAIState::Watching;
        }
    }
    else
    {
        int32 RandVal = FMath::RandRange(0, 100);
        if (RandVal < 50)
        {
            CurrentAIState = EBossAIState::Circling;
            CirclingDirection = FMath::RandBool() ? 1.0f : -1.0f;
        }
        else if (RandVal < 70)
        {
            CurrentAIState = EBossAIState::Watching;
        }
        else if (RandVal < 85)
        {
            CurrentAIState = EBossAIState::Approaching;
        }
        else
        {
            CurrentAIState = EBossAIState::SideDashing;
        }
    }

    GetWorldTimerManager().SetTimer(DecisionTimer, this, &AWorld2AIBossCharacter::MakeDecision, DecisionInterval, false);
}

void AWorld2AIBossCharacter::ExecuteState(float DeltaTime)
{
    // 공격 또는 쿨다운 상태일 때는 모든 움직임을 멈추고 아무것도 하지 않습니다.
    if (CurrentAIState == EBossAIState::Attacking || CurrentAIState == EBossAIState::AttackCooldown)
    {
        GetCharacterMovement()->StopMovementImmediately();
        
        // Attacking 상태이고, 아직 공격을 시작하지 않았다면 공격을 시작합니다.
        if (CurrentAIState == EBossAIState::Attacking && !bIsComboAttacking)
        {
            PerformAttack();
        }
    }
    else
    {
        // 공격/쿨다운이 아닌 다른 상태일 때만 움직임 로직을 실행합니다.
        FVector MoveDirection = FVector::ZeroVector;
        switch (CurrentAIState)
        {
            case EBossAIState::Watching:
                GetCharacterMovement()->MaxWalkSpeed = 0;
                break;

            case EBossAIState::Circling:
                GetCharacterMovement()->MaxWalkSpeed = StrafeSpeed;
                MoveDirection = GetActorRightVector() * CirclingDirection;
                AddMovementInput(MoveDirection);
                break;

            case EBossAIState::Approaching:
                if (!bIsCharging)
                {
                    bIsCharging = true;
                    GetCharacterMovement()->MaxWalkSpeed = ChargeSpeed;
                    GetWorldTimerManager().SetTimer(ChargeTimer, this, &AWorld2AIBossCharacter::OnChargeEnd, ChargeDuration, false);
                }
                MoveDirection = (PlayerCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
                AddMovementInput(MoveDirection);
                break;

            case EBossAIState::BackingOff:
                GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
                MoveDirection = (GetActorLocation() - PlayerCharacter->GetActorLocation()).GetSafeNormal();
                AddMovementInput(MoveDirection);
                if (!GetWorldTimerManager().IsTimerActive(DecisionTimer))
                {
                     GetWorldTimerManager().SetTimer(DecisionTimer, [this](){
                        CurrentAIState = EBossAIState::Watching;
                        MakeDecision();
                    }, RetreatDuration, false);
                }
                break;

            case EBossAIState::SideDashing:
                if (!bIsSideDashing)
                {
                    bIsSideDashing = true;
                    GetCharacterMovement()->MaxWalkSpeed = SideDashSpeed;
                    CirclingDirection = FMath::RandBool() ? 1.0f : -1.0f;
                    GetWorldTimerManager().SetTimer(SideDashTimer, this, &AWorld2AIBossCharacter::OnSideDashEnd, SideDashDuration, false);
                }
                MoveDirection = GetActorRightVector() * CirclingDirection;
                AddMovementInput(MoveDirection);
                break;
        }
    }
}

float AWorld2AIBossCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    if (ActualDamage > 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("HIT! World2AIBossCharacter '%s' took %.2f damage from %s."), *GetName(), ActualDamage, *DamageCauser->GetName());
    }
    return ActualDamage;
}

void AWorld2AIBossCharacter::OnChargeEnd()
{
    bIsCharging = false;
    CurrentAIState = EBossAIState::Watching;
}

void AWorld2AIBossCharacter::OnSideDashEnd()
{
    bIsSideDashing = false;
    CurrentAIState = EBossAIState::Watching;
}

void AWorld2AIBossCharacter::PerformAttack()
{
    if (AttackMontages.Num() == 0) return;

    bIsComboAttacking = true;
    // 콤보의 시작점을 무작위로 결정합니다. (0: 어택1, 1: 어택2, 2: 어택3)
    CurrentComboIndex = FMath::RandRange(0, AttackMontages.Num() - 1);

    UAnimMontage* AttackToPlay = AttackMontages[CurrentComboIndex];
    if (AttackToPlay)
    {
        PlayAnimMontage(AttackToPlay);
        FOnMontageEnded MontageEndedDelegate;
        MontageEndedDelegate.BindUObject(this, &AWorld2AIBossCharacter::OnAttackMontageEnded);
        GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndedDelegate, AttackToPlay);
    }
}

void AWorld2AIBossCharacter::CheckForNextCombo()
{
    if (!bCanDoNextCombo || !bIsComboAttacking) return;

    bCanDoNextCombo = false;

    // 콤보를 이어가기 전에, 플레이어가 여전히 공격 범위 내에 있는지 확인합니다.
    const float DistanceToPlayer = GetDistanceTo(PlayerCharacter);
    if (DistanceToPlayer > AttackRange + 50.0f) // 약간의 허용 오차를 줍니다.
    {
        UE_LOG(LogTemp, Log, TEXT("Player is out of range. Stopping combo."));
        OnAttackMontageEnded(nullptr, true);
        return;
    }

    // 콤보 확률에 따라 다음 공격을 결정합니다.
    if (FMath::RandRange(0, 100) < ComboChance)
    {
        CurrentComboIndex++;
        if (AttackMontages.IsValidIndex(CurrentComboIndex))
        {
            UAnimMontage* NextAttack = AttackMontages[CurrentComboIndex];
            if (NextAttack)
            {
                PlayAnimMontage(NextAttack);
                FOnMontageEnded MontageEndedDelegate;
                MontageEndedDelegate.BindUObject(this, &AWorld2AIBossCharacter::OnAttackMontageEnded);
                GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndedDelegate, NextAttack);
            }
        }
        else
        {
            bIsComboAttacking = false;
            OnAttackMontageEnded(nullptr, false);
        }
    }
    else
    {
        OnAttackMontageEnded(nullptr, true);
    }
}

void AWorld2AIBossCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    // 콤보를 시도할 수 있는 창이 열려있을 때는, 이 함수가 콤보를 방해하지 않도록 합니다.
    if (bCanDoNextCombo) return;

    // 이 함수가 중복 호출되는 것을 방지합니다.
    if (CurrentAIState == EBossAIState::AttackCooldown) return;

    UE_LOG(LogTemp, Log, TEXT("OnAttackMontageEnded called. bInterrupted: %s"), bInterrupted ? TEXT("true") : TEXT("false"));

    // 모든 공격 관련 상태를 확실하게 초기화합니다。
    bIsComboAttacking = false;
    bCanDoNextCombo = false;
    CurrentComboIndex = 0;

    // 공격 쿨다운 상태로 전환합니다。
    CurrentAIState = EBossAIState::AttackCooldown;

    // 쿨다운 후, 다시 의사결정을 시작하도록 타이머를 설정합니다。
    GetWorldTimerManager().SetTimer(DecisionTimer, this, &AWorld2AIBossCharacter::MakeDecision, AttackCooldownDuration, false);
}

void AWorld2AIBossCharacter::ActivateWeaponCollision()
{
    HitActors.Empty();

    if (LeftWeaponCollisionBox)
    {
        LeftWeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
    if (RightWeaponCollisionBox)
    {
        RightWeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
}

void AWorld2AIBossCharacter::DeactivateWeaponCollision()
{
    if (RightWeaponCollisionBox)
    {
        RightWeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    if (LeftWeaponCollisionBox)
    {
        LeftWeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}

void AWorld2AIBossCharacter::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor == this || HitActors.Contains(OtherActor)) return;

    APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
    if (Player)
    {
        UE_LOG(LogTemp, Warning, TEXT("Boss Weapon Hit Player!"));
        UGameplayStatics::ApplyDamage(Player, AttackDamage, GetController(), this, UDamageType::StaticClass());
        HitActors.Add(Player);
    }
}
