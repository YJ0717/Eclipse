#include "Destructible/PillarDestructible.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Field/FieldSystemObjects.h"

APillarDestructible::APillarDestructible()
{
    PrimaryActorTick.bCanEverTick = false;

    OuterShellComponent = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("OuterShell"));
    SetRootComponent(OuterShellComponent);

    InnerCoreComponent = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("InnerCore"));
    InnerCoreComponent->SetupAttachment(RootComponent);

    // --- 초기 설정 ---
    InnerCoreComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    InnerCoreComponent->SetSimulatePhysics(true);
    InnerCoreComponent->SetVisibility(false);
    OuterShellComponent->bNotifyBreaks = true;
    bIsOuterShellDestroyed = false;
    InnerCoreComponent->bNotifyBreaks = true;
    bInnerCoreBroken = false;
}

void APillarDestructible::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("BeginPlay Chaos Start"));
    OuterShellComponent->OnChaosBreakEvent.AddDynamic(this, &APillarDestructible::OnOuterShellBroken);

    InnerCoreComponent->OnChaosBreakEvent.AddDynamic(this, &APillarDestructible::OnInnerCoreBroken);
    InnerCoreComponent->SetSimulatePhysics(false);

}

void APillarDestructible::ApplyOuterShellDamage(FVector Location)
{
    if (bIsOuterShellDestroyed) return;
    UE_LOG(LogTemp, Warning, TEXT("ApplyOuterShellDamage - 이벤트 호출"));
    URadialFalloff* RadialFalloff = NewObject<URadialFalloff>();
    RadialFalloff->Magnitude = 1.0f;
    RadialFalloff->Radius = 75.f;
    RadialFalloff->Position = Location;

    UUniformScalar* UniformScalar = NewObject<UUniformScalar>();
    UniformScalar->Magnitude = OuterShellDamage;

    UOperatorField* OperatorField = NewObject<UOperatorField>();
    OperatorField->RightField = RadialFalloff;
    OperatorField->LeftField = UniformScalar;
    OperatorField->Operation = EFieldOperationType::Field_Multiply;

    InnerCoreComponent->SetSimulatePhysics(false);

    OuterShellComponent->ApplyPhysicsField(true, EGeometryCollectionPhysicsTypeEnum::Chaos_ExternalClusterStrain, nullptr, OperatorField);
}

void APillarDestructible::OnOuterShellBroken(const FChaosBreakEvent& BreakEvent)
{
    if (bIsOuterShellDestroyed) return;
    UE_LOG(LogTemp, Warning, TEXT("외벽 부셔짐 - 이벤트 호출"));

    bIsOuterShellDestroyed = true;



    // InnerCore Chaos 등록 순서
    InnerCoreComponent->SetVisibility(true);
    InnerCoreComponent->SetSimulatePhysics(true);
    UE_LOG(LogTemp, Warning, TEXT("카오스 다시 세팅"));
    // 이 시점에서 바인딩해야 이벤트가 작동함

    UE_LOG(LogTemp, Warning, TEXT("내벽 부셔짐 이벤트 바인딩"));
    // 외벽 제거 예약
    GetWorld()->GetTimerManager().SetTimer(HideOuterShellTimerHandle, this, &APillarDestructible::HideOuterShell, 5.0f, false);
}

void APillarDestructible::HideOuterShell()
{
    if (OuterShellComponent)
    {
        OuterShellComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        OuterShellComponent->SetVisibility(false);
    }


}




void APillarDestructible::ApplyInnerCoreDamage(FVector Location)
{
    if (!bIsOuterShellDestroyed) return;
    UE_LOG(LogTemp, Warning, TEXT("InnerCore 데미지 - 이벤트 호출됨"));
    URadialFalloff* RadialFalloff = NewObject<URadialFalloff>();
    RadialFalloff->Magnitude = 1.0f;
    RadialFalloff->Radius = 75.f;
    RadialFalloff->Position = Location;

    UUniformScalar* UniformScalar = NewObject<UUniformScalar>();
    UniformScalar->Magnitude = InnerCoreDamage;

    UOperatorField* OperatorField = NewObject<UOperatorField>();
    OperatorField->RightField = RadialFalloff;
    OperatorField->LeftField = UniformScalar;
    OperatorField->Operation = EFieldOperationType::Field_Multiply;

    InnerCoreComponent->ApplyPhysicsField(true, EGeometryCollectionPhysicsTypeEnum::Chaos_ExternalClusterStrain, nullptr, OperatorField);
}


void APillarDestructible::OnInnerCoreBroken(const FChaosBreakEvent& BreakEvent)
{

    UE_LOG(LogTemp, Warning, TEXT("InnerCore 파괴됨 - 이벤트 호출됨"));
    if (bInnerCoreBroken) return;
    bInnerCoreBroken = true;
    UE_LOG(LogTemp, Warning, TEXT("InnerCore 파괴됨"));
    // Trait UI 생성 및 띄우기
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC && TraitUIClass)
    {
        UUserWidget* TraitUI = CreateWidget<UUserWidget>(PC, TraitUIClass);
        if (TraitUI)
        {
            TraitUI->AddToViewport();
            PC->SetInputMode(FInputModeUIOnly());
            PC->bShowMouseCursor = true;
        }
    }
}