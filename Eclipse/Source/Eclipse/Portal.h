// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

UCLASS()
class ECLIPSE_API APortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    // 충돌 트리거
    UPROPERTY(VisibleAnywhere)
    class UBoxComponent* TriggerBox;

    // 다음 맵 이름 (여기 저장)
    UPROPERTY(EditAnywhere, Category = "Portal")
    FName LevelToLoad;

    // 도착 위치 Actor (레벨 안의 아무 액터)
    UPROPERTY(EditAnywhere, Category = "Portal")
    FName TargetActorName;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
