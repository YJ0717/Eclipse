#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "World2AIBossCharacter.generated.h"

// 보스의 AI 상태를 정의합니다.
UENUM(BlueprintType)
enum class EBossAIState : uint8
{
    Watching            UMETA(DisplayName = "Watching"),          // 플레이어 관망
    Circling            UMETA(DisplayName = "Circling"),          // 선회하며 각 재기
    Approaching         UMETA(DisplayName = "Approaching"),       // 접근
    BackingOff          UMETA(DisplayName = "BackingOff"),        // 거리 벌리기
    SideDashing         UMETA(DisplayName = "SideDashing"),       // 측면 대쉬
    Attacking           UMETA(DisplayName = "Attacking")           // 공격
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

    // 데미지를 받았을 때 호출될 함수 (AActor로부터 오버라이드)
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

private:
    // 플레이어와 마주보는 로직
    void FacePlayer(float DeltaTime);

    // 상태를 주기적으로 업데이트하는 함수
    void UpdateMovementState();

private:
    // AI의 핵심 의사결정을 내리는 함수
    void MakeDecision();

    // 각 상태에 따른 실제 움직임을 처리하는 함수
    void ExecuteState(float DeltaTime);

private:
    // 현재 AI 상태
    EBossAIState CurrentAIState;

    // 다음 의사결정까지의 시간을 제어하는 타이머
    FTimerHandle DecisionTimer;

protected:
    // 보스가 선회할 방향 (-1 or 1)
    float CirclingDirection;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float ChargeSpeed = 1500.0f; // 돌진 속도

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
    float ChargeDuration = 0.7f; // 돌진 지속 시간

    // 플레이어 캐릭터 참조
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    APlayerCharacter* PlayerCharacter;

    // --- AI 행동 파라미터 ---

    // 공격을 시작하는 거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
    float AttackRange = 300.0f;

    // 재배치 시 유지하려는 거리 (선회 거리)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
    float RepositionDistance = 700.0f;

    // 의사결정 주기 (초)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
    float DecisionInterval = 2.0f;

    // 후퇴하는 시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
    float RetreatDuration = 1.5f;

    // --- AI 속도 파라미터 ---

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float WalkSpeed = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float SprintSpeed = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float StrafeSpeed = 400.0f; // 좌우 이동 속도

protected:
    // 돌진 상태를 제어하기 위한 변수들
    bool bIsCharging;
    FTimerHandle ChargeTimer;
    void OnChargeEnd();

    // 측면 대쉬 상태를 제어하기 위한 변수들
    bool bIsSideDashing;
    FTimerHandle SideDashTimer;
    void OnSideDashEnd();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float SideDashSpeed = 1200.0f; // 측면 대쉬 속도

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
    float SideDashDuration = 0.4f; // 측면 대쉬 지속 시간

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float RotationSpeed = 5.0f;
};