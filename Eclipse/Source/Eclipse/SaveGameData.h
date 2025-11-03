
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameData.generated.h"

UCLASS()
class ECLIPSE_API USaveGameData : public USaveGame
{
    GENERATED_BODY()

public:
    // 저장할 플레이어의 이름
    UPROPERTY(VisibleAnywhere, Category = Basic)
    FString PlayerName;

    // 저장할 플레이어의 위치
    	UPROPERTY(VisibleAnywhere, Category = Basic)
        FVector PlayerLocation;
    
        // 저장할 플레이어의 HP
        UPROPERTY(VisibleAnywhere, Category = Basic)
        float PlayerHealth;
    // 기본값을 설정하는 생성자
    USaveGameData();
};
