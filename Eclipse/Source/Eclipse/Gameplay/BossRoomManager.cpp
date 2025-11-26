// BossRoomManager.cpp
#include "BossRoomManager.h"
#include "Sg1BossCharacter.h"
#include "Portal.h"
#include "Kismet/GameplayStatics.h"

ABossRoomManager::ABossRoomManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ABossRoomManager::BeginPlay()
{
    Super::BeginPlay();

    // Find the boss character in the level
    BossCharacter = Cast<ASg1BossCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), ASg1BossCharacter::StaticClass()));

    if (BossCharacter)
    {
        // Bind our handler to the boss's OnBossDied event
        BossCharacter->OnBossDied.AddDynamic(this, &ABossRoomManager::OnBossDiedHandler);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BossRoomManager: Could not find ASg1BossCharacter in the level."));
    }
}

void ABossRoomManager::OnBossDiedHandler(ASg1BossCharacter* DeadBoss)
{
    UE_LOG(LogTemp, Log, TEXT("Boss has died. Spawning portal."));

    if (PortalClass)
    {
        APortal* SpawnedPortal = GetWorld()->SpawnActor<APortal>(PortalClass, PortalSpawnTransform);
        if (SpawnedPortal)
        {
            SpawnedPortal->LevelToLoad = NextLevelName;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BossRoomManager: PortalClass is not set. Cannot spawn portal."));
    }
}
