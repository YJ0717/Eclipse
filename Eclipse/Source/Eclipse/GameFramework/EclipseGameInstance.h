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

    // 레벨 이동 시 플레이어의 HP를 임시 저장할 변수
    UPROPERTY(BlueprintReadWrite, Category = "SaveGame")
    float PlayerHealthOnTravel = 0.0f;
    //레벨이동시 스킬현재 무슨스킬인지 저장하는용도
    UPROPERTY(BlueprintReadWrite, Category = "SkillSave")
    int32 SkillAttackOnTravel = 0;
};
