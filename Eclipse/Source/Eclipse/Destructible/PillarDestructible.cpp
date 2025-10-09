#include "Destructible/PillarDestructible.h"
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
    InnerCoreComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    OuterShellComponent->bNotifyBreaks = true;
    bIsOuterShellDestroyed = false;
}

void APillarDestructible::BeginPlay()
{
    Super::BeginPlay();
    OuterShellComponent->OnChaosBreakEvent.AddDynamic(this, &APillarDestructible::OnOuterShellBroken);
}

void APillarDestructible::ApplyOuterShellDamage(FVector Location)
{
    if (bIsOuterShellDestroyed) return;

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

    OuterShellComponent->ApplyPhysicsField(true, EGeometryCollectionPhysicsTypeEnum::Chaos_ExternalClusterStrain, nullptr, OperatorField);
}

void APillarDestructible::OnOuterShellBroken(const FChaosBreakEvent& BreakEvent)
{
    if (bIsOuterShellDestroyed) return;

    bIsOuterShellDestroyed = true;
    // 내벽 콜리전은 즉시 활성화
    InnerCoreComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    // 겉껍질이 파괴된 후 5초 뒤에 원래 컴포넌트를 숨기고 콜리전 비활성화
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
