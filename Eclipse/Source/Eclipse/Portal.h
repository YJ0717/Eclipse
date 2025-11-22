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

    // 포탈 충돌 트리거
    UPROPERTY(VisibleAnywhere)
    class UBoxComponent* TriggerBox;

    // 로드할 레벨의 이름
    UPROPERTY(EditAnywhere, Category = "Portal")
    FName LevelToLoad;

    // 타겟 위치 액터 (플레이어가 포탈을 통과한 후 스폰될 위치를 나타내는 액터의 이름)
    UPROPERTY(EditAnywhere, Category = "Portal")
    FName TargetActorName;

    // 포탈이 다른 액터와 겹치기 시작할 때 호출되는 함수
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};