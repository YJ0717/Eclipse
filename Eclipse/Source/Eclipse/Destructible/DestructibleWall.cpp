#include "Destructible/DestructibleWall.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "PhysicsProxy/GeometryCollectionPhysicsProxy.h"
// 필수 헤더 추가: EFieldFalloffType 열거형이 여기에 정의되어 있습니다.
#include "Field/FieldSystemTypes.h"
#include "Field/FieldSystemObjects.h"

ADestructibleWall::ADestructibleWall()
{
    PrimaryActorTick.bCanEverTick = false;

    GeometryCollectionComponent = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
    SetRootComponent(GeometryCollectionComponent);
}

void ADestructibleWall::ApplyDamageAtLocation(const FVector& HitLocation, float DamageRadius, float DamageAmount)
{
    if (!GeometryCollectionComponent) return;

    // 1. 방사형 감쇠 필드 생성 (효과가 적용될 구체 영역)
    URadialFalloff* RadialFalloff = NewObject<URadialFalloff>();
    RadialFalloff->Magnitude = 1.0f; 
    RadialFalloff->Radius = DamageRadius;
    RadialFalloff->Position = HitLocation;
    // Falloff 타입 설정 제거 - 기본값이 Linear이므로, 식별자 문제를 회피합니다.

    // 2. 균일 스칼라 필드 생성 (적용할 효과의 종류와 값)
    UUniformScalar* UniformScalar = NewObject<UUniformScalar>();
    UniformScalar->Magnitude = DamageAmount;

    // 3. 연산자 필드 생성 (위 두 필드를 결합)
    UOperatorField* OperatorField = NewObject<UOperatorField>();
    OperatorField->RightField = RadialFalloff;
    OperatorField->LeftField = UniformScalar;
    OperatorField->Operation = EFieldOperationType::Field_Multiply;

    // 4. 필드를 지오메트리 컬렉션에 적용하여 외부 스트레인을 가함
    GeometryCollectionComponent->ApplyPhysicsField(
        true, 
        EGeometryCollectionPhysicsTypeEnum::Chaos_ExternalClusterStrain, 
        nullptr, 
        OperatorField
    );
}