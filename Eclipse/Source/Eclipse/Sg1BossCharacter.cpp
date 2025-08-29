
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
	LeftLegAttackCollision->SetupAttachment(GetMesh(), FName("L_Toe0Socket")); // 사용자가 알려준 소켓 이름으로 직접 지정
	
	RightLegAttackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightLegAttackCollision"));
	RightLegAttackCollision->SetupAttachment(GetMesh(), FName("R_Toe0Socket")); // 사용자가 알려준 소켓 이름으로 직접 지정

	HeadAttackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("HeadAttackCollision"));
	HeadAttackCollision->SetupAttachment(GetMesh(), FName("HeadSocket")); // 사용자가 알려준 소켓 이름으로 직접 지정

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

    CurrentHealth = MaxHealth;

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

float ASg1BossCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// 이미 죽었다면 데미지를 받지 않습니다.
	if (CurrentHealth <= 0.f) return 0.f;

	const float DamageTaken = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageTaken, 0.f, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("Sg1Boss took %.f damage. Current Health: %.f"), DamageTaken, CurrentHealth);

	if (CurrentHealth <= 0.f)
	{
		Die();
	}

	return DamageTaken;
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
	// 새로운 공격을 시작하기 전에 이전에 맞았던 액터 목록을 비웁니다.
	HitActors.Empty();

	switch (PartToActivate)
	{
		case EAttackPart::LeftLeg:
			UE_LOG(LogTemp, Log, TEXT("Activating Left Leg Collision"));
			LeftLegAttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			break;
		case EAttackPart::RightLeg:
			UE_LOG(LogTemp, Log, TEXT("Activating Right Leg Collision"));
			RightLegAttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			break;
		case EAttackPart::Head:
			UE_LOG(LogTemp, Log, TEXT("Activating Head Collision"));
			HeadAttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			break;
	}
}

void ASg1BossCharacter::DeactivateAttackCollision()
{
	UE_LOG(LogTemp, Log, TEXT("Deactivating All Attack Collision"));
	LeftLegAttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightLegAttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HeadAttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitActors.Empty(); // 확실하게 한번 더 비워줍니다.
}

void ASg1BossCharacter::OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 자기 자신이나, 유효하지 않은 액터는 무시합니다.
	if (!OtherActor || OtherActor == this) return;

	// 한 번의 공격 모션에서 같은 대상을 여러 번 때리는 것을 방지합니다.
	if (HitActors.Contains(OtherActor)) return;

	// 오버랩된 액터가 플레이어인지 확인합니다.
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sg1Boss HIT Player: %s with %s"), *Player->GetName(), *OverlappedComponent->GetName());
		HitActors.Add(OtherActor); // 맞춘 액터 목록에 추가
		UGameplayStatics::ApplyDamage(Player, AttackDamage, GetController(), this, UDamageType::StaticClass());
	}
}

// 이 함수는 AnimNotify에서 호출하여 공격이 끝났음을 알리고 상태를 초기화하는 데 사용합니다.
void ASg1BossCharacter::ResetAttackState()
{
	bIsAttacking = false;
}


