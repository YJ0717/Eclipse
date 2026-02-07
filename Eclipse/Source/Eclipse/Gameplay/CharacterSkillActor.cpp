#include "CharacterSkillActor.h"
#include "Sg1Monster1.h"
#include "Sg1Monster2.h"
#include "Sg1BossCharacter.h"
#include "Sg2Monster3.h"
#include "Sg2Monster4.h"
#include "Sg3BossCharacter.h"
#include "Sg4BossCharacter.h"
#include "World2Boss/World2AIBossCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "PlayerCharacter.h"
ACharacterSkillActor::ACharacterSkillActor()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	RootComponent = CollisionSphere;
	CollisionSphere->InitSphereRadius(100.f);
	CollisionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CollisionSphere->SetGenerateOverlapEvents(true);

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ACharacterSkillActor::OnSkillOverlap);
}

void ACharacterSkillActor::BeginPlay()
{
	Super::BeginPlay();
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (IsValid(Player))
	{
		Damage *= Player->SkillDamageRate;
		UE_LOG(LogTemp, Warning, TEXT("SkillActor Damage adjusted with SkillDamageRate: %f"), Player->SkillDamageRate);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SkillActor: Could not find valid PlayerCharacter!"));
	}
}

void ACharacterSkillActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MaxDistance > 0.f)
	{
		FVector MoveDir = GetActorForwardVector();
		FVector NewLocation = GetActorLocation() + MoveDir * MoveSpeed * DeltaTime;
		SetActorLocation(NewLocation);

		// 이동 거리 누적
		TraveledDistance += (MoveSpeed * DeltaTime);
		if (TraveledDistance >= MaxDistance)
		{
			Destroy();
		}
	}


}

void ACharacterSkillActor::OnSkillOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this)
		return;

	//  한 번만 맞도록
	if (HitActors.Contains(OtherActor))
		return;

	// ===== 몬스터 1 =====
	if (ASg1Monster1* Monster = Cast<ASg1Monster1>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Skill] Hit Monster1: %s"), *Monster->GetName());
		UGameplayStatics::ApplyDamage(Monster, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
		HitActors.Add(Monster);
	}

	// ===== 몬스터 2 =====
	else if (ASg1Monster2* Monster2 = Cast<ASg1Monster2>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Skill] Hit Monster2: %s"), *Monster2->GetName());
		UGameplayStatics::ApplyDamage(Monster2, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
		HitActors.Add(Monster2);
	}

	// ===== 몬스터 3 =====
	else if (ASg2Monster3* Monster3 = Cast<ASg2Monster3>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Skill] Hit Monster3: %s"), *Monster3->GetName());
		UGameplayStatics::ApplyDamage(Monster3, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
		HitActors.Add(Monster3);
	}

	// ===== 몬스터 4 =====
	else if (ASg2Monster4* Monster4 = Cast<ASg2Monster4>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Skill] Hit Monster4: %s"), *Monster4->GetName());
		UGameplayStatics::ApplyDamage(Monster4, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
		HitActors.Add(Monster4);
	}


	// ===== 보스 1 =====
	else if (ASg1BossCharacter* Boss = Cast<ASg1BossCharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Skill] Hit Boss: %s"), *Boss->GetName());
		UGameplayStatics::ApplyPointDamage(
			Boss,
			Damage,
			GetActorForwardVector(),
			SweepResult,
			GetInstigatorController(),
			this,
			UDamageType::StaticClass()
		);
		HitActors.Add(Boss);
	}

	// ===== 보스 2 (World2Boss) =====
	else if (AWorld2AIBossCharacter* World2Boss = Cast<AWorld2AIBossCharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Skill] Hit World2Boss: %s"), *World2Boss->GetName());
		UGameplayStatics::ApplyDamage(World2Boss, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
		HitActors.Add(World2Boss);
	}


	// ===== 보스 3 (World2Boss) =====
	else if (ASg3BossCharacter* World3Boss = Cast<ASg3BossCharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Skill] Hit World3Boss: %s"), *World3Boss->GetName());
		UGameplayStatics::ApplyDamage(World3Boss, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
		HitActors.Add(World3Boss);
	}

	else if (ASg4BossCharacter* World4Boss = Cast<ASg4BossCharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Skill] Hit World3Boss: %s"), *World4Boss->GetName());
		UGameplayStatics::ApplyDamage(World4Boss, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
		HitActors.Add(World4Boss);
	}

	// ? 중복 타격 방지 (한 번만 맞게)
	Destroy();
	
}