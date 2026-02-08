#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LavaDamageBox.generated.h"

class UBoxComponent;

UCLASS()
class ECLIPSE_API ALavaDamageBox : public AActor
{
	GENERATED_BODY()

public:
	ALavaDamageBox();

protected:
	virtual void BeginPlay() override;

	// 데미지 영역
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lava")
	UBoxComponent* DamageBox;

	// 1초당 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lava")
	float DamagePerSecond = 10.f;

	// 데미지 주는 주기(초) - 기본 1초
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lava")
	float DamageInterval = 1.0f;

	// 안에 들어와 있는 플레이어들(일단 싱글이어도 안전하게)
	UPROPERTY()
	TSet<AActor*> OverlappingPlayers;

	FTimerHandle DamageTimerHandle;

	UFUNCTION()
	void OnBoxBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	void DealDamageTick();
};