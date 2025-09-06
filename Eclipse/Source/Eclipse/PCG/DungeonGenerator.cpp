
// Fill out your copyright notice in the Description page of Project Settings.

#include "PCG/DungeonGenerator.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/KismetMathLibrary.h"

ADungeonGenerator::ADungeonGenerator()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ADungeonGenerator::BeginPlay()
{
    Super::BeginPlay();
}

void ADungeonGenerator::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ADungeonGenerator::GenerateDungeon()
{
    // 생성 전 이전 던전 정리
    ClearDungeon();

    if (!FloorMesh || !WallMesh || !CorridorMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("Dungeon meshes are not set!"));
        return;
    }

    FVector CurrentLocation = GetActorLocation();
    TArray<FVector> RoomCenters;
    
    for (int32 i = 0; i < NumRooms; ++i)
    {
        FIntPoint RoomSize;
        RoomSize.X = FMath::RandRange(RoomSizeMin.X, RoomSizeMax.X);
        RoomSize.Y = FMath::RandRange(RoomSizeMin.Y, RoomSizeMax.Y);

        SpawnRoom(CurrentLocation, RoomSize);
        RoomCenters.Add(CurrentLocation);

        // 다음 방으로 이동하기 위한 복도 생성
        FVector CorridorStart = CurrentLocation;
        FVector Direction;
        switch (FMath::RandRange(0, 3))
        {
            case 0: Direction = FVector::ForwardVector; break; // North
            case 1: Direction = FVector::BackwardVector; break; // South
            case 2: Direction = FVector::RightVector; break; // East
            case 3: Direction = FVector::LeftVector; break; // West
        }

        FVector CorridorEnd = CorridorStart + Direction * (FMath::Max(RoomSize.X, RoomSize.Y) / 2.0f + CorridorLength) * TileSize;
        SpawnCorridor(CorridorStart, CorridorEnd);
        
        CurrentLocation = CorridorEnd;
    }
}

void ADungeonGenerator::ClearDungeon()
{
    for (AActor* Actor : SpawnedActors)
    {
        if (Actor)
        {
            Actor->Destroy();
        }
    }
    SpawnedActors.Empty();
}

void ADungeonGenerator::SpawnRoom(const FVector& Center, const FIntPoint& Size)
{
    FVector StartOffset = FVector(Size.X / -2.0f, Size.Y / -2.0f, 0) * TileSize;

    for (int32 x = 0; x < Size.X; ++x)
    {
        for (int32 y = 0; y < Size.Y; ++y)
        {
            FVector TileLocation = Center + StartOffset + FVector(x * TileSize, y * TileSize, 0);
            
            // 바닥 생성
            SpawnMesh(FloorMesh, TileLocation, FRotator::ZeroRotator);

            // 벽 생성 (가장자리)
            if (x == 0) SpawnMesh(WallMesh, TileLocation - FVector(0, TileSize / 2, 0), FRotator(0, -90, 0));
            if (x == Size.X - 1) SpawnMesh(WallMesh, TileLocation + FVector(0, TileSize / 2, 0), FRotator(0, 90, 0));
            if (y == 0) SpawnMesh(WallMesh, TileLocation - FVector(TileSize / 2, 0, 0), FRotator::ZeroRotator);
            if (y == Size.Y - 1) SpawnMesh(WallMesh, TileLocation + FVector(TileSize / 2, 0, 0), FRotator(0, 180, 0));
        }
    }
}

void ADungeonGenerator::SpawnCorridor(const FVector& Start, const FVector& End)
{
    FVector Direction = (End - Start).GetSafeNormal();
    float Distance = FVector::Dist(Start, End);
    int32 NumTiles = FMath::CeilToInt(Distance / TileSize);

    for (int32 i = 0; i < NumTiles; ++i)
    {
        FVector Location = Start + Direction * i * TileSize;
        SpawnMesh(CorridorMesh, Location, Direction.Rotation());
    }
}

void ADungeonGenerator::SpawnMesh(UStaticMesh* Mesh, const FVector& Location, const FRotator& Rotation)
{
    if (UWorld* World = GetWorld())
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        AStaticMeshActor* MeshActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, Rotation, SpawnParams);
        if (MeshActor)
        {
            MeshActor->GetStaticMeshComponent()->SetStaticMesh(Mesh);
            SpawnedActors.Add(MeshActor);
        }
    }
}
