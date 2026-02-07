// Fill out your copyright notice in the Description page of Project Settings.

#include "Sg1BossCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Player/PlayerCharacter.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "GameFramework/DamageType.h"
#include "Engine/DamageEvents.h"
#include "Gameplay/BossRoomManager.h"



ASg1BossCharacter::ASg1BossCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // 캐릭터의 기본 캡슐이 플레이어를 막도록 설정합니다.
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

    // --- 공격 충돌 박스 생성 및 설정 ---
    LeftLegAttackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftLegAttackCollision"));
    LeftLegAttackCollision->SetupAttachment(GetMesh(), FName("L_Toe0Socket"));

    RightLegAttackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightLegAttackCollision"));
    RightLegAttackCollision->SetupAttachment(GetMesh(), FName("R_Toe0Socket"));

    HeadAttackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("HeadAttackCollision"));
    HeadAttackCollision->SetupAttachment(GetMesh(), FName("HeadSocket"));


    LeftLegHitCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftLegHitCollision"));
    LeftLegHitCollision->SetupAttachment(GetMesh(), FName("L_Toe0Socket"));
    RightLegHitCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightLegHitCollision"));
    RightLegHitCollision->SetupAttachment(GetMesh(), FName("R_Toe0Socket"));
    HeadHitCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("HeadHitCollision"));
    HeadHitCollision->SetupAttachment(GetMesh(), FName("HeadSocket"));

    TArray<UBoxComponent*> AttackCollisions = { LeftLegAttackCollision, RightLegAttackCollision, HeadAttackCollision };
    for (UBoxComponent* Collision : AttackCollisions)
    {
        Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Collision->SetCollisionObjectType(ECC_WorldDynamic);
        Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
        Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);   // 플레이어 캐릭터와 충돌 (보스 공격)
        Collision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
        Collision->SetGenerateOverlapEvents(true);

    }
    HitCollisions = { LeftLegHitCollision, RightLegHitCollision, HeadHitCollision };
    for (UBoxComponent* Collision : HitCollisions)
    {
        Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        Collision->SetCollisionObjectType(ECC_WorldDynamic);
        Collision->SetCollisionResponseToAllChannels(ECR_Ignore);

        // 무기(Box)가 WorldDynamic이니까 여기서 Block 해줘야 Trace/Overlap에서 Comp로 잡힘
        Collision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);

        // 필요하면 Pawn도 Block
        // Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

        Collision->SetGenerateOverlapEvents(true);
    }

    GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void ASg1BossCharacter::BeginPlay()
{
    Super::BeginPlay();

    // --- DEBUGGING ATTACHMENT ---
    UE_LOG(LogTemp, Error, TEXT("--- Checking Collision Attachments ---"));

    FString LeftLegParent = LeftLegAttackCollision->GetAttachParent() ? LeftLegAttackCollision->GetAttachParent()->GetName() : TEXT("NONE");
    FName LeftLegSocket = LeftLegAttackCollision->GetAttachSocketName();
    UE_LOG(LogTemp, Error, TEXT("LeftLegAttackCollision: Parent=[%s], Socket=[%s]"), *LeftLegParent, *LeftLegSocket.ToString());

    FString RightLegParent = RightLegAttackCollision->GetAttachParent() ? RightLegAttackCollision->GetAttachParent()->GetName() : TEXT("NONE");
    FName RightLegSocket = RightLegAttackCollision->GetAttachSocketName();
    UE_LOG(LogTemp, Error, TEXT("RightLegAttackCollision: Parent=[%s], Socket=[%s]"), *RightLegParent, *RightLegSocket.ToString());

    FString HeadParent = HeadAttackCollision->GetAttachParent() ? HeadAttackCollision->GetAttachParent()->GetName() : TEXT("NONE");
    FName HeadSocket = HeadAttackCollision->GetAttachSocketName();
    UE_LOG(LogTemp, Error, TEXT("HeadAttackCollision: Parent=[%s], Socket=[%s]"), *HeadParent, *HeadSocket.ToString());

    UE_LOG(LogTemp, Error, TEXT("--- End Attachment Check ---"));
    // --- END DEBUGGING ---







    CurrentHealth = MaxHealth;

    // 부위별 HP 초기화 (총합이 MaxHealth와 일치하도록)
    PartHealth.Add(EAttackPart::LeftLeg, MaxHealth / 3.0);
    PartHealth.Add(EAttackPart::RightLeg, MaxHealth / 3.0);
    PartHealth.Add(EAttackPart::Head, MaxHealth / 3.0);

    TotalCurrentHealth = 0.f;
    for (auto& Elem : PartHealth)
    {
        TotalCurrentHealth += Elem.Value;
    }

    PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!PlayerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Sg1BossCharacter: PlayerCharacter not found!"));
    }
    float Total = 0.f;
    for (auto& Elem : PartHealth)
    {
        Total += Elem.Value;
        UE_LOG(LogTemp, Warning, TEXT("Part [%d] = %.2f"), (int32)Elem.Key, Elem.Value);
    }
    UE_LOG(LogTemp, Warning, TEXT("TotalCurrentHealth = %.2f | Expected MaxHealth = %.2f"), Total, MaxHealth);

    // 각 공격 충돌 박스의 오버랩 이벤트에 함수를 연결(바인딩)합니다.
    LeftLegAttackCollision->OnComponentBeginOverlap.AddDynamic(this, &ASg1BossCharacter::OnAttackOverlapBegin);
    RightLegAttackCollision->OnComponentBeginOverlap.AddDynamic(this, &ASg1BossCharacter::OnAttackOverlapBegin);
    HeadAttackCollision->OnComponentBeginOverlap.AddDynamic(this, &ASg1BossCharacter::OnAttackOverlapBegin);


    LeftLegHitCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    RightLegHitCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    HeadHitCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

}

void ASg1BossCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);





    if (PlayerCharacter && CanAttack() && CurrentHealth > 0)
    {
        DecideAttackPattern();
    }
}


float ASg1BossCharacter::TakeDamage(float DamageAmount,
    FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser)
{
    Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (TotalCurrentHealth <= 0.f) return 0.f;

    EAttackPart HitPart = EAttackPart::None;

    if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
    {
        const FPointDamageEvent* PointEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
        UPrimitiveComponent* HitComp = PointEvent->HitInfo.Component.Get();
        UE_LOG(LogTemp, Warning, TEXT("[Boss::TakeDamage] HitComp=%s"), HitComp ? *HitComp->GetName() : TEXT("NULL"));

        if (HitComp == LeftLegHitCollision)      HitPart = EAttackPart::LeftLeg;
        else if (HitComp == RightLegHitCollision)HitPart = EAttackPart::RightLeg;
        else if (HitComp == HeadHitCollision)    HitPart = EAttackPart::Head;
    }

    if (PartHealth.Contains(HitPart))
    {
        float OldHP = PartHealth[HitPart];
        PartHealth[HitPart] = FMath::Clamp(OldHP - DamageAmount, 0.f, MaxHealth);

        // 총합 갱신
        TotalCurrentHealth = 0.f;
        for (auto& Elem : PartHealth) TotalCurrentHealth += Elem.Value;

        UE_LOG(LogTemp, Warning, TEXT("[Boss::TakeDamage] Part=%d %.1f -> %.1f | Total=%.1f"),
            (int32)HitPart, OldHP, PartHealth[HitPart], TotalCurrentHealth);

        if (PartHealth[HitPart] <= 0.f)
        {

            UE_LOG(LogTemp, Warning, TEXT("[Boss] Part %d destroyed (disable its attacks)."), (int32)HitPart);

            OnPartDestroyed_BP(HitPart);
        }
        if (TotalCurrentHealth <= 0.f) Die();
    }

    return DamageAmount;
}


bool ASg1BossCharacter::CanAttack() const
{
    return !bIsAttacking && (GetWorld()->GetTimeSeconds() - LastAttackTime > AttackCooldown);
}

void ASg1BossCharacter::DecideAttackPattern()
{
    if (!PlayerCharacter) return;

    const FVector BossToPlayer = PlayerCharacter->GetActorLocation() - GetActorLocation();
    const FVector BossForwardVector = GetActorForwardVector();
    const FVector BossRightVector = GetActorRightVector();
    const FVector BossToPlayerNormalized = BossToPlayer.GetSafeNormal();
    const float DotProductWithRight = FVector::DotProduct(BossToPlayerNormalized, BossRightVector);
    const float DotProductWithForward = FVector::DotProduct(BossToPlayerNormalized, BossForwardVector);
    const float AngleWithForward = FMath::Acos(DotProductWithForward);

    if (FMath::RadiansToDegrees(AngleWithForward) < 30.0f)
    {
        PerformHeadAttack();
    }
    else if (DotProductWithRight > 0)
    {
        PerformRightLegAttack();
    }
    else
    {
        PerformLeftLegAttack();
    }
}

void ASg1BossCharacter::PerformLeftLegAttack()
{
    if (PartHealth.Contains(EAttackPart::LeftLeg) && PartHealth[EAttackPart::LeftLeg] <= 0.f)
    {
        return;
    }

    if (GetMesh()->GetAnimInstance() && GetMesh()->GetAnimInstance()->Montage_IsPlaying(nullptr))
    {
        return; // 이미 다른 몽타주가 재생 중이면 시작하지 않음
    }

    if (LeftLegAttackMontage)
    {
        CurrentAttackPart = EAttackPart::LeftLeg;
        bIsAttacking = true;
        LastAttackTime = GetWorld()->GetTimeSeconds();
        PlayAnimMontage(LeftLegAttackMontage);
    }
}

void ASg1BossCharacter::PerformRightLegAttack()
{

    if (PartHealth.Contains(EAttackPart::RightLeg) && PartHealth[EAttackPart::RightLeg] <= 0.f)
    {
        return;
    }
    if (GetMesh()->GetAnimInstance() && GetMesh()->GetAnimInstance()->Montage_IsPlaying(nullptr))
    {
        return; // 이미 다른 몽타주가 재생 중이면 시작하지 않음
    }

    if (RightLegAttackMontage)
    {
        CurrentAttackPart = EAttackPart::RightLeg;
        bIsAttacking = true;
        LastAttackTime = GetWorld()->GetTimeSeconds();
        PlayAnimMontage(RightLegAttackMontage);
    }
}

void ASg1BossCharacter::PerformHeadAttack()
{
    if (PartHealth.Contains(EAttackPart::Head) && PartHealth[EAttackPart::Head] <= 0.f)
    {
        return;
    }
    if (GetMesh()->GetAnimInstance() && GetMesh()->GetAnimInstance()->Montage_IsPlaying(nullptr))
    {
        return; // 이미 다른 몽TA주가 재생 중이면 시작하지 않음
    }

    if (HeadAttackMontage)
    {
        CurrentAttackPart = EAttackPart::Head;
        bIsAttacking = true;
        LastAttackTime = GetWorld()->GetTimeSeconds();
        PlayAnimMontage(HeadAttackMontage);
    }
}

void ASg1BossCharacter::Die()
{
    UE_LOG(LogTemp, Warning, TEXT("Sg1Boss has died."));

    // AI 로직을 멈춥니다.
    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController && AIController->BrainComponent)
    {
        AIController->BrainComponent->StopLogic("Death");
    }

    // 충돌을 없애고 래그돌을 활성화합니다.
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
    GetMesh()->SetSimulatePhysics(true);

    DieUI();

    if (DeathMontage)
    {
        PlayAnimMontage(DeathMontage);
    }

    // 보스가 죽었음을 알리는 델리게이트를 호출합니다.
    OnBossDied.Broadcast(this);

    // BossRoomManager에 알립니다
    TArray<AActor*> FoundManagers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABossRoomManager::StaticClass(), FoundManagers);
    for (AActor* Manager : FoundManagers)
    {
        if (ABossRoomManager* BossManager = Cast<ABossRoomManager>(Manager))
        {
            BossManager->OnBossDefeated();
            UE_LOG(LogTemp, Warning, TEXT("Sg1Boss: Notified BossRoomManager"));
            break; // 첫 번째 매니저만 호출
        }
    }

    // 10초 뒤에 액터를 파괴합니다.
    SetLifeSpan(10.0f);
}

void ASg1BossCharacter::ActivateAttackCollision(EAttackPart PartToActivate)
{
    // 새로운 공격을 시작하기 전에 이전에 맞았던 액터 목록을 비웁니다.
    HitActors.Empty();

    LeftLegAttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RightLegAttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    HeadAttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    switch (CurrentAttackPart)
    {
    case EAttackPart::LeftLeg:
        LeftLegAttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        break;
    case EAttackPart::RightLeg:
        RightLegAttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        break;
    case EAttackPart::Head:
        HeadAttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        break;
    case EAttackPart::None:
        UE_LOG(LogTemp, Error, TEXT("CurrentAttackPart is None! No collision was activated."));
        break;
    }
}

void ASg1BossCharacter::DeactivateAttackCollision()
{
    UE_LOG(LogTemp, Warning, TEXT("--- DeactivateAttackCollision CALLED. Turning ALL boxes OFF. ---"));
    LeftLegAttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RightLegAttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    HeadAttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    HitActors.Empty(); // 확실하게 한번 더 비워줍니다.
}

void ASg1BossCharacter::OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this) return;
    if (HitActors.Contains(OtherActor)) return;

    APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
    if (Player)
    {
        UE_LOG(LogTemp, Warning, TEXT("Sg1Boss HIT Player: %s with %s"), *Player->GetName(), *OverlappedComponent->GetName());
        HitActors.Add(OtherActor);

        UGameplayStatics::ApplyDamage(Player, AttackDamage, GetController(), this, UDamageType::StaticClass());
    }
}



// 이 함수는 AnimNotify에서 호출하여 공격이 끝났음을 알리고 상태를 초기화하는 데 사용합니다.
void ASg1BossCharacter::ResetAttackState()
{
    bIsAttacking = false;
}

EAttackPart ASg1BossCharacter::DetectHitPart(FName BoneName)
{
    if (BoneName == FName("Head") || BoneName == FName("HeadSocket"))
    {
        return EAttackPart::Head;
    }
    else if (BoneName == FName("L_Toe0Socket"))
    {
        return EAttackPart::LeftLeg;
    }
    else if (BoneName == FName("R_Toe0Socket"))
    {
        return EAttackPart::RightLeg;
    }

    // 기본값 (못 찾았을 경우 머리로 처리)
    return EAttackPart::None;
}

