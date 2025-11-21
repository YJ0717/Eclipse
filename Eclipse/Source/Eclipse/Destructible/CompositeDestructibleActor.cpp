// Copyright Epic Games, Inc. All Rights Reserved.

#include "Destructible/CompositeDestructibleActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"

ACompositeDestructibleActor::ACompositeDestructibleActor()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Root);

	GC1_Component = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GC1"));
	GC1_Component->SetupAttachment(GetRootComponent());
	GC1_Component->bNotifyBreaks = true;

	GC2_Component = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GC2"));
	GC2_Component->SetupAttachment(GetRootComponent());
	GC2_Component->bNotifyBreaks = true;

	GC3_StaticComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GC3_Static"));
	GC3_StaticComponent->SetupAttachment(GetRootComponent());

	GC3_DestructibleComponent = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GC3_Destructible"));
	GC3_DestructibleComponent->SetupAttachment(GetRootComponent());
}

void ACompositeDestructibleActor::BeginPlay()
{
	Super::BeginPlay();

	// 블루프린트에서 할당한 에셋을 컴포넌트에 적용
	if (GC3_GeometryCollection)
	{
		GC3_DestructibleComponent->SetRestCollection(GC3_GeometryCollection);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("오류: 'GC3 Geometry Collection' 에셋이 블루프린트에 할당되지 않음!"), *GetName());
	}

	// --- 초기 상태 설정 ---
	// GC1 & GC2는 파괴 가능 상태로 시작
	GC1_Component->SetSimulatePhysics(true);
	GC1_Component->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	GC2_Component->SetSimulatePhysics(true);
	GC2_Component->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// GC3는 보이는 스태틱 메시 상태로 시작
	GC3_StaticComponent->SetVisibility(true);
	GC3_StaticComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// GC3의 파괴 가능 버전은 숨겨진 비활성 상태로 시작
	GC3_DestructibleComponent->SetVisibility(false);
	GC3_DestructibleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GC3_DestructibleComponent->SetSimulatePhysics(false);

	// --- 파괴 이벤트 바인딩 ---
	GC1_Component->OnChaosBreakEvent.AddDynamic(this, &ACompositeDestructibleActor::OnGC1_Broken);
	GC2_Component->OnChaosBreakEvent.AddDynamic(this, &ACompositeDestructibleActor::OnGC2_Broken);
}

void ACompositeDestructibleActor::OnGC1_Broken(const FChaosBreakEvent& BreakEvent)
{
	if (bIsGC1_Destroyed) return; // 중복 호출 방지
	
	bIsGC1_Destroyed = true;
	ActivateDestructibleGC3(); // GC3 활성화 조건 확인
}

void ACompositeDestructibleActor::OnGC2_Broken(const FChaosBreakEvent& BreakEvent)
{
	if (bIsGC2_Destroyed) return; // 중복 호출 방지

	bIsGC2_Destroyed = true;
	ActivateDestructibleGC3(); // GC3 활성화 조건 확인
}

// GC1과 GC2가 모두 파괴되면 GC3의 파괴 가능 버전을 활성화
void ACompositeDestructibleActor::ActivateDestructibleGC3()
{
	if (bIsGC1_Destroyed && bIsGC2_Destroyed)
	{
		if (GC3_StaticComponent)
		{
			// 스태틱 메시 숨기기 및 비활성화
			GC3_StaticComponent->SetVisibility(false);
			GC3_StaticComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		if (GC3_DestructibleComponent)
		{
			// 파괴 가능 버전 보이기 및 활성화
			GC3_DestructibleComponent->SetVisibility(true);
			GC3_DestructibleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			GC3_DestructibleComponent->SetSimulatePhysics(true);
		}
	}
}