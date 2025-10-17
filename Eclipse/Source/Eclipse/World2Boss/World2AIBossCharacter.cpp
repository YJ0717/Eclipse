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

    WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
    WeaponCollisionBox->SetupAttachment(GetMesh(), FName("hand_rSocket"));
    WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WeaponCollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    WeaponCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    WeaponCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AWorld2AIBossCharacter::BeginPlay()
{
    Super::BeginPlay();

    PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    if (WeaponCollisionBox)
    {
        WeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AWorld2AIBossCharacter::OnWeaponOverlap);
    }

    GetWorldTimerManager().SetTimer(DecisionTimer, this, &AWorld2AIBossCharacter::MakeDecision, 0.5f, false);
}

void AWorld2AIBossCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

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

    if (Distance <= AttackRange)
    {
        if (FMath::RandRange(0, 100) < 60)
        {
            CurrentAIState = EBossAIState::Attacking;
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

        case EBossAIState::Attacking:
            if (!bIsComboAttacking)
            {
                PerformAttack();
            }
            break;

        case EBossAIState::AttackCooldown:
            GetCharacterMovement()->StopMovementImmediately();
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
    CurrentComboIndex = 0;

    UAnimMontage* FirstAttack = AttackMontages[CurrentComboIndex];
    if (FirstAttack)
    {
        PlayAnimMontage(FirstAttack);
        FOnMontageEnded MontageEndedDelegate;
        MontageEndedDelegate.BindUObject(this, &AWorld2AIBossCharacter::OnAttackMontageEnded);
        GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndedDelegate, FirstAttack);
    }
}

void AWorld2AIBossCharacter::CheckForNextCombo()
{
    UE_LOG(LogTemp, Log, TEXT("CheckForNextCombo called. bCanDoNextCombo: %s, bIsComboAttacking: %s"), bCanDoNextCombo ? TEXT("true") : TEXT("false"), bIsComboAttacking ? TEXT("true") : TEXT("false"));

    if (!bCanDoNextCombo || !bIsComboAttacking) return;

    bCanDoNextCombo = false; // 콤보 결정 기회는 한 번 뿐

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
        // 콤보를 이어가지 않기로 결정하면 공격 종료 처리를 직접 호출합니다.
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
    if (WeaponCollisionBox)
    {
        HitActors.Empty();
        WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
}

void AWorld2AIBossCharacter::DeactivateWeaponCollision()
{
    if (WeaponCollisionBox)
    {
        WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
