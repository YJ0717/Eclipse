// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chaos/ChaosGameplayEventDispatcher.h"
#include "GeometryCollection/GeometryCollectionObject.h"
#include "CompositeDestructibleActor.generated.h"

class UGeometryCollectionComponent;
class UStaticMeshComponent;
class UStaticMesh;

UCLASS()
class ECLIPSE_API ACompositeDestructibleActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ACompositeDestructibleActor();

protected:
	virtual void BeginPlay() override;

private:
	// 블루프린트에서 할당할 에셋
	UPROPERTY(EditAnywhere, Category = "Destructible Components | Assets")
	TObjectPtr<UGeometryCollection> GC3_GeometryCollection; // GC3 파괴 상태용 Geometry Collection 에셋

	// 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Destructible Components")
	TObjectPtr<UGeometryCollectionComponent> GC1_Component;

	UPROPERTY(VisibleAnywhere, Category = "Destructible Components")
	TObjectPtr<UGeometryCollectionComponent> GC2_Component;

	// GC3의 초기, 파괴 불가능 상태용 Static Mesh
	UPROPERTY(VisibleAnywhere, Category = "Destructible Components")
	TObjectPtr<UStaticMeshComponent> GC3_StaticComponent;

	// GC3의 파괴 가능 상태용 Geometry Collection. 처음에는 숨겨짐
	UPROPERTY(VisibleAnywhere, Category = "Destructible Components")
	TObjectPtr<UGeometryCollectionComponent> GC3_DestructibleComponent;

	// 상태 변수
	bool bIsGC1_Destroyed = false;
	bool bIsGC2_Destroyed = false;

	// 로직 함수
	UFUNCTION()
	void OnGC1_Broken(const FChaosBreakEvent& BreakEvent);

	UFUNCTION()
	void OnGC2_Broken(const FChaosBreakEvent& BreakEvent);
	
	// GC3 활성화 조건 확인 및 실행
	void ActivateDestructibleGC3();
};
