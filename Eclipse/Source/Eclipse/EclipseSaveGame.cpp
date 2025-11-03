
#include "EclipseSaveGame.h"
#include "SaveGameData.h"
#include "Kismet/GameplayStatics.h"

// USaveGameData 생성자 구현
USaveGameData::USaveGameData()
{
    PlayerName = TEXT("DefaultPlayerName");
    PlayerLocation = FVector(0.f);
}

// 게임 저장 함수 구현
void UEclipseSaveGame::SaveGame(const FString& SlotName, int32 UserIndex, const FString& PlayerName, const FVector& PlayerLocation, float PlayerHealth)
{
    // 새로운 SaveGame 객체를 생성하거나 기존 것을 불러옵니다.
    USaveGameData* SaveGameInstance = Cast<USaveGameData>(UGameplayStatics::CreateSaveGameObject(USaveGameData::StaticClass()));
    if (SaveGameInstance)
    {
        // 데이터를 SaveGame 객체에 저장합니다.
        SaveGameInstance->PlayerName = PlayerName;
        SaveGameInstance->PlayerLocation = PlayerLocation;
        SaveGameInstance->PlayerHealth = PlayerHealth; // HP 저장

        // 파일로 저장합니다.
        UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName, UserIndex);
    }
}

// 게임 불러오기 함수 구현
USaveGameData* UEclipseSaveGame::LoadGame(const FString& SlotName, int32 UserIndex)
{
    // 지정된 슬롯에 세이브 파일이 있는지 확인합니다.
    if (UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
    {
        // 파일에서 SaveGame 객체를 불러옵니다.
        USaveGameData* LoadedGame = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
        return LoadedGame;
    }

    return nullptr; // 저장된 게임이 없으면 nullptr을 반환합니다.
}
