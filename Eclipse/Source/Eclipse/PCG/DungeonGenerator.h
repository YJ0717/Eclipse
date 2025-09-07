
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonGenerator.generated.h"

UENUM(BlueprintType)
enum class EWallType : uint8
{
    Bottom,
    Top,
    Left,
    Right
};

UCLASS()
class ECLIPSE_API ADungeonGenerator : public AActor
{
    GENERATED_BODY()

public:
    ADungeonGenerator();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // 던전 생성에 사용할 스태틱 메시
    UPROPERTY(EditAnywhere, Category = "Dungeon | Meshes")
    UStaticMesh* FloorMesh;

    UPROPERTY(EditAnywhere, Category = "Dungeon | Meshes")
    UStaticMesh* WallMesh;

    UPROPERTY(EditAnywhere, Category = "Dungeon | Meshes")
    UStaticMesh* DoorMesh;

    UPROPERTY(EditAnywhere, Category = "Dungeon | Meshes")
    UStaticMesh* CorridorMesh;

    // 던전 생성 설정
    UPROPERTY(EditAnywhere, Category = "Dungeon | Settings")
    int32 NumRooms = 10;

    UPROPERTY(EditAnywhere, Category = "Dungeon | Settings")
    FIntPoint RoomSizeMin = FIntPoint(3, 3);

    UPROPERTY(EditAnywhere, Category = "Dungeon | Settings")
    FIntPoint RoomSizeMax = FIntPoint(5, 5);
    
    UPROPERTY(EditAnywhere, Category = "Dungeon | Settings")
    int32 CorridorLength = 5;

    UPROPERTY(EditAnywhere, Category = "Dungeon | Settings")
    float TileSize = 500.0f; // 메시 하나의 크기 (cm)

    UPROPERTY(EditAnywhere, Category = "Dungeon | Settings")
    FVector FloorOffset = FVector(0.0f, 0.0f, 0.0f); // 바닥 타일 위치 조정 오프셋 (cm)

    UPROPERTY(EditAnywhere, Category = "Dungeon | Settings")
    FVector WallOffset = FVector(-250.0f, -250.0f, 0.0f); // 벽 타일 위치 조정 오프셋 (cm)

    UPROPERTY(EditAnywhere, Category = "Dungeon | Settings")
    FVector FloorScale = FVector(1.0f, 1.0f, 1.0f); // 바닥 타일 스케일 조정

    // 에디터에서 던전을 생성하는 버튼
    UFUNCTION(CallInEditor, Category = "Dungeon | Generation")
    void GenerateDungeon();

    // 생성된 던전을 정리하는 버튼
    UFUNCTION(CallInEditor, Category = "Dungeon | Generation")
    void ClearDungeon();

private:
    void SpawnRoom(const FVector& Center, const FIntPoint& Size);
    void SpawnCorridor(const FVector& Start, const FVector& End);
    AActor* SpawnMesh(UStaticMesh* Mesh, const FVector& Location, const FRotator& Rotation, const FVector& Scale = FVector(1.0f, 1.0f, 1.0f));
    FRotator GetWallRotationForLocation(int32 WallX, int32 WallY, const FIntPoint& Size, EWallType WallType); // New helper function

    bool DoesRoomOverlap(const FIntRect& ProposedRoomRect, const TArray<FIntRect>& ExistingRoomRects); // New helper function declaration

    FIntPoint WorldToGrid(const FVector& WorldLocation); // New helper function declaration

    // 생성된 액터들을 추적하기 위한 배열
    UPROPERTY()
    TArray<AActor*> SpawnedActors;

    // 생성된 방의 영역을 추적하기 위한 배열
    TArray<FIntRect> OccupiedRoomAreas;

    // 생성된 벽의 그리드 위치를 추적하기 위한 배열
    TMap<FIntPoint, AActor*> OccupiedWallLocations;
};
