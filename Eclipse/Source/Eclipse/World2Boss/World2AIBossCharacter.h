#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "World2AIBossCharacter.generated.h"

// 보스의 움직임 상태를 정의합니다.
UENUM(BlueprintType)
enum class EBossMovementState : uint8
{
    Idle    UMETA(DisplayName = "Idle"),
    Walking UMETA(DisplayName = "Walking"),
    Sprinting UMETA(DisplayName = "Sprinting")
};

class APlayerCharacter;

UCLASS()
class ECLIPSE_API AWorld2AIBossCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AWorld2AIBossCharacter();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

private:
    // 플레이어와 마주보는 로직
    void FacePlayer(float DeltaTime);

    // 플레이어의 움직임에 맞춰 심리전 이동
    void PsychologicalMove(float DeltaTime);

    // 상태를 주기적으로 업데이트하는 함수
    void UpdateMovementState();

private:
    // 상태 변경을 위한 타이머
    FTimerHandle StateChangeTimer;

protected:
    // 보스의 현재 움직임 상태 (블루프린트에서 읽기 가능)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|State")
    EBossMovementState CurrentMovementState;

protected:
    // 플레이어 캐릭터에 대한 참조
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    APlayerCharacter* PlayerCharacter;

    // --- AI Movement Parameters ---

    // 플레이어와 유지하려는 최소/최대 거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float MinIdealDistance = 450.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float MaxIdealDistance = 700.0f;

    // 걷기 및 뛰기 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float WalkSpeed = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float SprintSpeed = 400.0f;
    
    // 좌우로 움직이는 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float StrafeSpeed = 250.0f;

    // 회전 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float RotationSpeed = 5.0f;

    // 상태 변경 주기 (최소/최대 시간)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|State")
    float MinStateChangeTime = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|State")
    float MaxStateChangeTime = 5.0f;
};