#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "World2AIBossCharacter.generated.h"

UENUM(BlueprintType)
enum class EBossAIState : uint8
{
    Watching            UMETA(DisplayName = "Watching"),
    Circling            UMETA(DisplayName = "Circling"),
    Approaching         UMETA(DisplayName = "Approaching"),
    BackingOff          UMETA(DisplayName = "BackingOff"),
    SideDashing         UMETA(DisplayName = "SideDashing"),       // 측면 대쉬
    Attacking           UMETA(DisplayName = "Attacking"),           // 공격
    AttackCooldown      UMETA(DisplayName = "Attack Cooldown")      // 공격 후 딜레이
};

class APlayerCharacter;

UCLASS()
class ECLIPSE_API AWorld2AIBossCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AWorld2AIBossCharacter();

public:
    virtual void Tick(float DeltaTime) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
    virtual void BeginPlay() override;

private:
    // AI 로직 함수
    void FacePlayer(float DeltaTime);
    void MakeDecision();
    void ExecuteState(float DeltaTime);
    void PerformAttack();

    // 충돌 및 상태 제어 함수
    UFUNCTION(BlueprintCallable)
    void ActivateWeaponCollision();

    UFUNCTION(BlueprintCallable)
    void DeactivateWeaponCollision();
    UFUNCTION()
    void OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    void OnChargeEnd();
    void OnSideDashEnd();

public:
    // 콤보 가능 여부를 외부에서 제어할 수 있도록 public으로 변경
    bool bCanDoNextCombo;
    bool bIsComboAttacking;
    int32 CurrentComboIndex;
    UFUNCTION(BlueprintCallable)
    void CheckForNextCombo();
    void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float AttackCooldownDuration = 2.0f; // 공격 후 딜레이 시간

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    int32 ComboChance = 70; // 다음 콤보로 이어갈 확률 (0-100)

    // AI 상태 변수
    EBossAIState CurrentAIState;
    FTimerHandle DecisionTimer;
    TArray<AActor*> HitActors;

    // 상태 제어 플래그
    bool bIsCharging;
    bool bIsSideDashing;
    FTimerHandle ChargeTimer;
    FTimerHandle SideDashTimer;
    float CirclingDirection;

protected:
    // --- 플레이어 참조 ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    APlayerCharacter* PlayerCharacter;

    // --- AI 행동 파라미터 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
    float AttackRange = 300.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
    float RepositionDistance = 700.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
    float DecisionInterval = 2.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
    float RetreatDuration = 1.5f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
    float ChargeDuration = 0.7f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
    float SideDashDuration = 0.4f;

    // --- AI 속도 파라미터 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float WalkSpeed = 150.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float SprintSpeed = 500.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float StrafeSpeed = 400.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float ChargeSpeed = 1500.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float SideDashSpeed = 1200.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
    float RotationSpeed = 5.0f;

    // --- AI 전투 파라미터 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float AttackDamage = 20.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat")
    TArray<class UAnimMontage*> AttackMontages;
    UPROPERTY(VisibleDefaultsOnly, Category = "AI|Combat")
    class UBoxComponent* RightWeaponCollisionBox;

    UPROPERTY(VisibleDefaultsOnly, Category = "AI|Combat")
    class UBoxComponent* LeftWeaponCollisionBox;
};