#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EclipseGameInstance.generated.h"

UCLASS()
class ECLIPSE_API UEclipseGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    // 생성자
    UEclipseGameInstance();

    // 이 변수가 true이면 게임 시작 시 로드를 시도합니다.
    UPROPERTY(BlueprintReadWrite, Category = "SaveGame")
    bool bShouldLoadGame;
};
