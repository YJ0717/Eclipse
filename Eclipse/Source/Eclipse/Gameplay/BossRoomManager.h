// BossRoomManager.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossRoomManager.generated.h"

class ASg1BossCharacter;
class APortal;

UCLASS()
class ECLIPSE_API ABossRoomManager : public AActor
{
    GENERATED_BODY()

public:
    ABossRoomManager();

protected:
    virtual void BeginPlay() override;

private:
    // The class of Portal to spawn after the boss is defeated.
    UPROPERTY(EditAnywhere, Category = "BossRoomManager")
    TSubclassOf<APortal> PortalClass;

    // The transform where the portal will be spawned.
    UPROPERTY(EditAnywhere, Category = "BossRoomManager")
    FTransform PortalSpawnTransform;

    // The name of the level to load when the portal is used.
    UPROPERTY(EditAnywhere, Category = "BossRoomManager")
    FName NextLevelName;

    // A reference to the boss in the level.
    UPROPERTY()
    ASg1BossCharacter* BossCharacter;

    // Function to be called when the boss dies.
    UFUNCTION()
    void OnBossDiedHandler(ASg1BossCharacter* DeadBoss);
};
