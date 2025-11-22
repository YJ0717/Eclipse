#include "EnemySkillActor.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"

AEnemySkillActor::AEnemySkillActor()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	RootComponent = CollisionSphere;
	CollisionSphere->InitSphereRadius(100.f);
	CollisionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CollisionSphere->SetGenerateOverlapEvents(true);
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemySkillActor::OnSkillOverlap);
}

void AEnemySkillActor::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemySkillActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MaxDistance > 0.f)
	{
		FVector MoveDir = GetActorForwardVector();
		FVector NewLocation = GetActorLocation() + MoveDir * MoveSpeed * DeltaTime;
		SetActorLocation(NewLocation);

		TraveledDistance += (MoveSpeed * DeltaTime);
		if (TraveledDistance >= MaxDistance)
		{
			Destroy();
		}
	}
}

void AEnemySkillActor::OnSkillOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this)
		return;

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("[EnemySkill] Hit Player: %s"), *Player->GetName());
		UGameplayStatics::ApplyDamage(Player, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
	}
}