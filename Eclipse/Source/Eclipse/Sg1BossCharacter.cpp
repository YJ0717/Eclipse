
// Fill out your copyright notice in the Description page of Project Settings.

#include "Sg1BossCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "PlayerCharacter.h"
#include "AIController.h" // AI 컨트롤러를 사용하기 위해 추가
#include "BrainComponent.h" // 브레인 컴포넌트를 사용하기 위해 추가

ASg1BossCharacter::ASg1BossCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

	// 캐릭터의 기본 캡슐이 플레이어를 막도록 설정합니다.
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

	// --- 공격 충돌 박스 생성 및 설정 ---
	LeftLegAttackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftLegAttackCollision"));
	LeftLegAttackCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("L_Toe0Socket"));
	
	RightLegAttackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightLegAttackCollision"));
	RightLegAttackCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("R_Toe0Socket"));

	HeadAttackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("HeadAttackCollision"));
	HeadAttackCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("HeadSocket"));

	TArray<UBoxComponent*> AttackCollisions = { LeftLegAttackCollision, RightLegAttackCollision, HeadAttackCollision };
	for (UBoxComponent* Collision : AttackCollisions)
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Collision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		Collision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		Collision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
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
	PartHealth.Add(EAttackPart::LeftLeg, MaxHealth / 3);
	PartHealth.Add(EAttackPart::RightLeg, MaxHealth / 3);
	PartHealth.Add(EAttackPart::Head, MaxHealth / 3);
	
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

	// 각 공격 충돌 박스의 오버랩 이벤트에 함수를 연결(바인딩)합니다.
	LeftLegAttackCollision->OnComponentBeginOverlap.AddDynamic(this, &ASg1BossCharacter::OnAttackOverlapBegin);
	RightLegAttackCollision->OnComponentBeginOverlap.AddDynamic(this, &ASg1BossCharacter::OnAttackOverlapBegin);
	HeadAttackCollision->OnComponentBeginOverlap.AddDynamic(this, &ASg1BossCharacter::OnAttackOverlapBegin);
}

void ASg1BossCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (PlayerCharacter && CanAttack() && CurrentHealth > 0)
    {
        DecideAttackPattern();
    }
}

float ASg1BossCharacter::TakeDamage(float DamageAmount, EAttackPart HitPart, AActor* DamageCauser)
{
    // 이미 죽었다면 무시
    if (TotalCurrentHealth <= 0.f) return 0.f;

    // 해당 부위 HP 감소
    if (PartHealth.Contains(HitPart))
    {
        PartHealth[HitPart] = FMath::Clamp(PartHealth[HitPart] - DamageAmount, 0.f, MaxHealth);

        // 총합 갱신
        TotalCurrentHealth = 0.f;
        for (auto& Elem : PartHealth)
            TotalCurrentHealth += Elem.Value;

        UE_LOG(LogTemp, Warning, TEXT("Boss hit! Part %d HP: %.1f, Total HP: %.1f"), (int32)HitPart, PartHealth[HitPart], TotalCurrentHealth);

        if (TotalCurrentHealth <= 0.f)
            Die();
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
    const FVector BossRightVector = FVector::CrossProduct(FVector::UpVector, BossForwardVector);
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
    if (GetMesh()->GetAnimInstance() && GetMesh()->GetAnimInstance()->Montage_IsPlaying(nullptr))
    {
        return; // 이미 다른 몽타주가 재생 중이면 시작하지 않음
    }

    if (LeftLegAttackMontage)
    {
        bIsAttacking = true;
        LastAttackTime = GetWorld()->GetTimeSeconds();
        UE_LOG(LogTemp, Log, TEXT("Performing Left Leg Attack"));
        PlayAnimMontage(LeftLegAttackMontage);
    }
}

void ASg1BossCharacter::PerformRightLegAttack()
{
    if (GetMesh()->GetAnimInstance() && GetMesh()->GetAnimInstance()->Montage_IsPlaying(nullptr))
    {
        return; // 이미 다른 몽타주가 재생 중이면 시작하지 않음
    }

    if (RightLegAttackMontage)
    {
        bIsAttacking = true;
        LastAttackTime = GetWorld()->GetTimeSeconds();
        UE_LOG(LogTemp, Log, TEXT("Performing Right Leg Attack"));
        PlayAnimMontage(RightLegAttackMontage);
    }
}

void ASg1BossCharacter::PerformHeadAttack()
{
    if (GetMesh()->GetAnimInstance() && GetMesh()->GetAnimInstance()->Montage_IsPlaying(nullptr))
    {
        return; // 이미 다른 몽TA주가 재생 중이면 시작하지 않음
    }

    if (HeadAttackMontage)
    {
        bIsAttacking = true;
        LastAttackTime = GetWorld()->GetTimeSeconds();
        UE_LOG(LogTemp, Log, TEXT("Performing Head Attack"));
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

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}

	// 10초 뒤에 액터를 파괴합니다.
	SetLifeSpan(10.0f);
}

void ASg1BossCharacter::ActivateAttackCollision(EAttackPart PartToActivate)
{
	UE_LOG(LogTemp, Warning, TEXT("--- ActivateAttackCollision CALLED ---"));
	// 새로운 공격을 시작하기 전에 이전에 맞았던 액터 목록을 비웁니다.
	HitActors.Empty();

	// 안전을 위해 모든 공격 콜리전을 먼저 끕니다.
	UE_LOG(LogTemp, Warning, TEXT("Deactivating ALL boxes before activation."));
	LeftLegAttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightLegAttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HeadAttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 이제 필요한 콜리전만 다시 켭니다.
	switch (PartToActivate)
	{
		case EAttackPart::LeftLeg:
			UE_LOG(LogTemp, Warning, TEXT("ACTIVATING Left Leg Collision ONLY."));
			LeftLegAttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			break;
		case EAttackPart::RightLeg:
			UE_LOG(LogTemp, Warning, TEXT("ACTIVATING Right Leg Collision ONLY."));
			RightLegAttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			break;
		case EAttackPart::Head:
			UE_LOG(LogTemp, Warning, TEXT("ACTIVATING Head Collision ONLY."));
			HeadAttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
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
        HitActors.Add(OtherActor);

        // 어느 부위가 맞았는지 기록
        if (OverlappedComponent == LeftLegAttackCollision) LastHitPart = EAttackPart::LeftLeg;
        else if (OverlappedComponent == RightLegAttackCollision) LastHitPart = EAttackPart::RightLeg;
        else if (OverlappedComponent == HeadAttackCollision) LastHitPart = EAttackPart::Head;

        // TakeDamage 호출 (플레이어 공격력 전달)
        TakeDamage(Player->AttackDamage, LastHitPart, Player);
    }
}
// 이 함수는 AnimNotify에서 호출하여 공격이 끝났음을 알리고 상태를 초기화하는 데 사용합니다.
void ASg1BossCharacter::ResetAttackState()
{
	bIsAttacking = false;
}


