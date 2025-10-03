
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DestructibleWall.generated.h"

class UGeometryCollectionComponent;

UCLASS()
class ECLIPSE_API ADestructibleWall : public AActor
{
    GENERATED_BODY()

public:
    ADestructibleWall();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UGeometryCollectionComponent* GeometryCollectionComponent;

    UFUNCTION(BlueprintCallable, Category = "Destruction")
    void ApplyDamageAtLocation(const FVector& HitLocation, float DamageRadius, float DamageAmount);
};
