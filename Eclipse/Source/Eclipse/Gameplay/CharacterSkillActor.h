#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "CharacterSkillActor.generated.h"

UCLASS()
class ECLIPSE_API ACharacterSkillActor : public AActor
{
	GENERATED_BODY()

public:
	ACharacterSkillActor();



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

	//한 번 맞은 액터 저장 (중복 타격 방지)
	UPROPERTY()
	TArray<AActor*> HitActors;

	// 이동 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Movement")
	float MoveSpeed = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Movement")
	float MaxDistance = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Movement")
	float TraveledDistance;

	//충돌 시 데미지 처리
	UFUNCTION()
	void OnSkillOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

private:
	
};
