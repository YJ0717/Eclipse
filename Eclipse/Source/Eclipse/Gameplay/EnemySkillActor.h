#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "EnemySkillActor.generated.h"

UCLASS()
class ECLIPSE_API AEnemySkillActor : public AActor
{
	GENERATED_BODY()

public:
	AEnemySkillActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	USphereComponent* CollisionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float Damage = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float LifeTime = 3.f;

	UPROPERTY()
	TArray<AActor*> HitActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Movement")
	float MoveSpeed = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Movement")
	float MaxDistance = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Movement")
	float TraveledDistance;

	UFUNCTION()
	void OnSkillOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
};
