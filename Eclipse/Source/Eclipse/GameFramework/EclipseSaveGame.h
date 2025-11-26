
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EclipseSaveGame.generated.h"

class USaveGameData;

UCLASS()
class ECLIPSE_API UEclipseSaveGame : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // 게임을 저장하는 함수
    UFUNCTION(BlueprintCallable, Category = "SaveGame")
    static void SaveGame(const FString& SlotName, int32 UserIndex, const FString& PlayerName, const FVector& PlayerLocation, float PlayerHealth, const FString& LevelName);

    // 게임을 불러오는 함수
    UFUNCTION(BlueprintCallable, Category = "SaveGame")
    static USaveGameData* LoadGame(const FString& SlotName, int32 UserIndex);
};
