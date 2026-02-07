// BossRoomManager.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossRoomManager.generated.h"

class APortal;

/**
 * 범용 보스룸 매니저
 * 어떤 보스든 죽으면 포탈을 생성합니다
 */
UCLASS()
class ECLIPSE_API ABossRoomManager : public AActor
{
    GENERATED_BODY()

public:
    ABossRoomManager();

protected:
    virtual void BeginPlay() override;

private:
    // 포탈 클래스
    UPROPERTY(EditAnywhere, Category = "BossRoomManager")
    TSubclassOf<APortal> PortalClass;

    // 포탈 생성 위치
    UPROPERTY(EditAnywhere, Category = "BossRoomManager")
    FTransform PortalSpawnTransform;

    // 다음 레벨 이름
    UPROPERTY(EditAnywhere, Category = "BossRoomManager")
    FName NextLevelName;

    // 이 방의 보스 액터 (레벨에 배치된 보스를 여기에 할당)
    UPROPERTY(EditInstanceOnly, Category = "BossRoomManager")
    AActor* BossActor;

    // 포탈이 이미 생성되었는지 추적
    bool bPortalSpawned = false;

public:
    // 보스가 죽었을 때 외부에서 호출할 함수
    UFUNCTION(BlueprintCallable, Category = "BossRoomManager")
    void OnBossDefeated();
};
