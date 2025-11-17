#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chaos/ChaosGameplayEventDispatcher.h"
#include "PillarDestructible.generated.h"

class UGeometryCollectionComponent;

UCLASS()
class ECLIPSE_API APillarDestructible : public AActor
{
    GENERATED_BODY()

public:
    APillarDestructible();

    // UI 클래스 지정
    UPROPERTY(EditAnywhere, Category = "Trait")
    TSubclassOf<class UUserWidget> TraitUIClass;

    // 중복 방지
    bool bInnerCoreBroken = false;



    // 외부에서 데미지를 주기 위한 함수들
    void ApplyOuterShellDamage(FVector Location);
    void ApplyInnerCoreDamage(FVector Location);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UGeometryCollectionComponent> OuterShellComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UGeometryCollectionComponent> InnerCoreComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float OuterShellDamage = 100000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float InnerCoreDamage = 500000.f;

    bool bIsOuterShellDestroyed;

private:
    FTimerHandle HideOuterShellTimerHandle;

    UFUNCTION()
    void HideOuterShell();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnInnerCoreBroken(const FChaosBreakEvent& BreakEvent);

    UFUNCTION()
    void OnOuterShellBroken(const FChaosBreakEvent& BreakEvent);
};
