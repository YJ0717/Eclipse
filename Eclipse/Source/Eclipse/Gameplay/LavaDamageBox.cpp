// LavaDamageBox.cpp

#include "LavaDamageBox.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// 네 프로젝트 플레이어 클래스 이름 맞춰서 include
#include "PlayerCharacter.h"

ALavaDamageBox::ALavaDamageBox()
{
	PrimaryActorTick.bCanEverTick = false;

	DamageBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageBox"));
	RootComponent = DamageBox;

	// 충돌 설정: 플레이어만 Overlap
	DamageBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DamageBox->SetCollisionObjectType(ECC_WorldDynamic);
	DamageBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	DamageBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	DamageBox->SetGenerateOverlapEvents(true);

	// 박스 크기 기본값 (필요하면 BP에서 조절)
	DamageBox->SetBoxExtent(FVector(200.f, 200.f, 50.f));
}

void ALavaDamageBox::BeginPlay()
{
	Super::BeginPlay();

	DamageBox->OnComponentBeginOverlap.AddDynamic(this, &ALavaDamageBox::OnBoxBeginOverlap);
	DamageBox->OnComponentEndOverlap.AddDynamic(this, &ALavaDamageBox::OnBoxEndOverlap);

	// 1초마다 틱
	if (DamageInterval > 0.f)
	{
		GetWorldTimerManager().SetTimer(DamageTimerHandle, this, &ALavaDamageBox::DealDamageTick, DamageInterval, true);
	}
}

void ALavaDamageBox::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;

	// 플레이어만
	if (Cast<APlayerCharacter>(OtherActor))
	{
		OverlappingPlayers.Add(OtherActor);
	}
}

void ALavaDamageBox::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor == this) return;

	OverlappingPlayers.Remove(OtherActor);
}

void ALavaDamageBox::DealDamageTick()
{
	if (OverlappingPlayers.Num() == 0) return;

	const float DamageThisTick = DamagePerSecond * DamageInterval;

	for (auto It = OverlappingPlayers.CreateIterator(); It; ++It)
	{
		AActor* Actor = *It;
		if (!IsValid(Actor))
		{
			It.RemoveCurrent();
			continue;
		}

		APlayerCharacter* Player = Cast<APlayerCharacter>(Actor);
		if (!Player)
		{
			It.RemoveCurrent();
			continue;
		}

		UGameplayStatics::ApplyDamage(Player, DamageThisTick, nullptr, this, UDamageType::StaticClass());
	}
}
