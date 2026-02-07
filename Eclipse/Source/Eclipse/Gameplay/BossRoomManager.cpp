// BossRoomManager.cpp
#include "BossRoomManager.h"
#include "Portal.h"
#include "Kismet/GameplayStatics.h"

ABossRoomManager::ABossRoomManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ABossRoomManager::BeginPlay()
{
    Super::BeginPlay();

    if (!BossActor)
    {
        UE_LOG(LogTemp, Error, TEXT("BossRoomManager: BossActor is not assigned! Please assign the boss in the level."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BossRoomManager: Monitoring boss '%s'"), *BossActor->GetName());
    }
}

void ABossRoomManager::OnBossDefeated()
{
    if (bPortalSpawned)
    {
        UE_LOG(LogTemp, Warning, TEXT("BossRoomManager: Portal already spawned, ignoring."));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("BossRoomManager: Boss defeated! Spawning portal."));

    if (PortalClass)
    {
        APortal* SpawnedPortal = GetWorld()->SpawnActor<APortal>(PortalClass, PortalSpawnTransform);
        if (SpawnedPortal)
        {
            SpawnedPortal->LevelToLoad = NextLevelName;
            bPortalSpawned = true;
            UE_LOG(LogTemp, Warning, TEXT("BossRoomManager: Portal spawned successfully! Next level: %s"), *NextLevelName.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("BossRoomManager: Failed to spawn portal!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BossRoomManager: PortalClass is not set."));
    }
}