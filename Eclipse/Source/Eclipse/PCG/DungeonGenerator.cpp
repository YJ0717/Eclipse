
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
    OccupiedRoomAreas.Empty(); // 방 영역 초기화

    if (!FloorMesh || !WallMesh || !CorridorMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("Dungeon meshes are not set!"));
        return;
    }

    FVector CurrentLocation = GetActorLocation();
    TArray<FVector> RoomCenters; // 이 배열은 더 이상 충돌 감지에 사용되지 않습니다.
    
    // 첫 번째 방 생성
    FIntPoint InitialRoomSize;
    InitialRoomSize.X = FMath::RandRange(RoomSizeMin.X, RoomSizeMax.X);
    InitialRoomSize.Y = FMath::RandRange(RoomSizeMin.Y, RoomSizeMax.Y);

    SpawnRoom(CurrentLocation, InitialRoomSize);
    
    // 첫 번째 방의 FIntRect 계산 및 추가
    FVector InitialFirstTileCenterOffset = FVector(
        -(InitialRoomSize.X / 2.0f - 0.5f) * TileSize,
        -(InitialRoomSize.Y / 2.0f - 0.5f) * TileSize,
        0
    );
    FVector InitialFirstTileWorldCenter = CurrentLocation + InitialFirstTileCenterOffset;

    int32 InitialRoomMinX = FMath::RoundToInt(InitialFirstTileWorldCenter.X / TileSize - 0.5f);
    int32 InitialRoomMinY = FMath::RoundToInt(InitialFirstTileWorldCenter.Y / TileSize - 0.5f);

    FIntRect InitialRoomRect(
        InitialRoomMinX,
        InitialRoomMinY,
        InitialRoomMinX + InitialRoomSize.X,
        InitialRoomMinY + InitialRoomSize.Y
    );
    OccupiedRoomAreas.Add(InitialRoomRect);

    // 나머지 방 생성
    for (int32 i = 1; i < NumRooms; ++i)
    {
        FVector ProposedNextRoomLocation = CurrentLocation;
        FIntPoint ProposedRoomSize;
        FIntRect ProposedRoomRect;
        bool bRoomPlaced = false;
        int32 MaxRetries = 100; // 무한 루프 방지를 위한 최대 재시도 횟수

        for (int32 RetryCount = 0; RetryCount < MaxRetries; ++RetryCount)
        {
            ProposedRoomSize.X = FMath::RandRange(RoomSizeMin.X, RoomSizeMax.X);
            ProposedRoomSize.Y = FMath::RandRange(RoomSizeMin.Y, RoomSizeMax.Y);

            // 다음 방으로 이동하기 위한 복도 생성 방향
            FVector Direction;
            switch (FMath::RandRange(0, 3))
            {
                case 0: Direction = FVector::ForwardVector; break; // North
                case 1: Direction = FVector::BackwardVector; break; // South
                case 2: Direction = FVector::RightVector; break; // East
                case 3: Direction = FVector::LeftVector; break; // West
            }

            ProposedNextRoomLocation = CurrentLocation + Direction * (FMath::Max(ProposedRoomSize.X, ProposedRoomSize.Y) / 2.0f + CorridorLength) * TileSize;

            // 제안된 방의 FIntRect 계산
            FVector ProposedFirstTileCenterOffset = FVector(
                -(ProposedRoomSize.X / 2.0f - 0.5f) * TileSize,
                -(ProposedRoomSize.Y / 2.0f - 0.5f) * TileSize,
                0
            );
            FVector ProposedFirstTileWorldCenter = ProposedNextRoomLocation + ProposedFirstTileCenterOffset;

            int32 ProposedRoomMinX = FMath::RoundToInt(ProposedFirstTileWorldCenter.X / TileSize - 0.5f);
            int32 ProposedRoomMinY = FMath::RoundToInt(ProposedFirstTileWorldCenter.Y / TileSize - 0.5f);

            ProposedRoomRect = FIntRect(
                ProposedRoomMinX,
                ProposedRoomMinY,
                ProposedRoomMinX + ProposedRoomSize.X,
                ProposedRoomMinY + ProposedRoomSize.Y
            );

            if (!DoesRoomOverlap(ProposedRoomRect, OccupiedRoomAreas))
            {
                bRoomPlaced = true;
                break; // 겹치지 않는 위치를 찾음
            }
        }

        if (bRoomPlaced)
        {
            SpawnRoom(ProposedNextRoomLocation, ProposedRoomSize);
            OccupiedRoomAreas.Add(ProposedRoomRect);
            CurrentLocation = ProposedNextRoomLocation;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to place room after %d retries. Skipping room."), MaxRetries);
            // 이 경우 CurrentLocation이 업데이트되지 않아 다음 방이 현재 방과 겹칠 수 있습니다.
            // 더 견고한 던전 생성기를 위해서는 백트래킹 로직이 필요할 수 있습니다.
        }
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
    // Calculate the center of the (0,0) tile relative to the room's overall center
    FVector FirstTileCenterOffset = FVector(
        -(Size.X / 2.0f - 0.5f) * TileSize,
        -(Size.Y / 2.0f - 0.5f) * TileSize,
        0
    );

    // Calculate the actual center of the (0,0) tile in world coordinates
    FVector FirstTileWorldCenter = Center + FirstTileCenterOffset;

    // Spawn Floor Tiles
    for (int32 x = 0; x < Size.X; ++x)
    {
        for (int32 y = 0; y < Size.Y; ++y)
        {
            FVector FloorTileLocation = FirstTileWorldCenter + FVector(x * TileSize, y * TileSize, 0);
            SpawnMesh(FloorMesh, FloorTileLocation, FRotator::ZeroRotator);
        }
    }

            // 벽 생성 (가장자리)
            // X축을 따라 벽 생성 (상단 및 하단)
            for (int32 WallX = 0; WallX < Size.X; ++WallX)
            {
                // 하단 벽
                FVector BottomWallLocation = FirstTileWorldCenter + FVector(WallX * TileSize, -0.5f * TileSize, 0);
                FRotator BottomWallRotation = GetWallRotationForLocation(WallX, 0, Size, EWallType::Bottom);
                SpawnMesh(WallMesh, BottomWallLocation, BottomWallRotation);

                // 상단 벽
                FVector TopWallLocation = FirstTileWorldCenter + FVector(WallX * TileSize, (Size.Y - 0.5f) * TileSize, 0);
                FRotator TopWallRotation = GetWallRotationForLocation(WallX, Size.Y - 1, Size, EWallType::Top);
                SpawnMesh(WallMesh, TopWallLocation, TopWallRotation);
            }

            // Y축을 따라 벽 생성 (왼쪽 및 오른쪽) - 코너 포함 (겹침 발생)
            for (int32 WallY = 0; WallY < Size.Y; ++WallY) // WallY = 0 (하단) 및 WallY = Size.Y - 1 (상단)도 포함
            {
                // 왼쪽 벽
                FVector LeftWallLocation = FirstTileWorldCenter + FVector(-0.5f * TileSize, WallY * TileSize, 0);
                FRotator LeftWallRotation = GetWallRotationForLocation(0, WallY, Size, EWallType::Left);
                SpawnMesh(WallMesh, LeftWallLocation, LeftWallRotation);

                // 오른쪽 벽
                FVector RightWallLocation = FirstTileWorldCenter + FVector((Size.X - 0.5f) * TileSize, WallY * TileSize, 0);
                FRotator RightWallRotation = GetWallRotationForLocation(Size.X - 1, WallY, Size, EWallType::Right);
                SpawnMesh(WallMesh, RightWallLocation, RightWallRotation);
            }
}

FRotator ADungeonGenerator::GetWallRotationForLocation(int32 WallX, int32 WallY, const FIntPoint& Size, EWallType WallType)
{
    FRotator WallRotation = FRotator::ZeroRotator; // Default rotation

    bool isLeftCol = (WallX == 0);
    bool isRightCol = (WallX == Size.X - 1);
    bool isBottomRow = (WallY == 0);
    bool isTopRow = (WallY == Size.Y - 1);

    switch (WallType)
    {
        case EWallType::Bottom:
            WallRotation = FRotator::ZeroRotator; // Default for straight bottom wall
            if (isLeftCol) { // 남서 코너 (Bottom-Left)
                WallRotation = FRotator(0, 310, 0);
            } else if (isRightCol) { // 남동 코너 (Bottom-Right)
                WallRotation = FRotator(0, 0, 0);
            }
            break;

        case EWallType::Top:
            WallRotation = FRotator(0, 180, 0); // Default for straight top wall
            if (isLeftCol) { // 북서 코너 (Top-Left)
                WallRotation = FRotator(0, 180, 0);
            } else if (isRightCol) { // 북동 코너 (Top-Right)
                WallRotation = FRotator(0, 120, 0);
            }
            break;

        case EWallType::Left:
            WallRotation = FRotator(0, -90, 0); // Default for straight left wall
            if (isBottomRow) { // 남서 코너 (Bottom-Left) - 겹침 발생
                WallRotation = FRotator(0, 270, 0);
            } else if (isTopRow) { // 북서 코너 (Top-Left) - 겹침 발생
                WallRotation = FRotator(0, 210, 0);
            }
            break;

        case EWallType::Right:
            WallRotation = FRotator(0, 90, 0); // Default for straight right wall
            if (isBottomRow) { // 남동 코너 (Bottom-Right) - 겹침 발생
                WallRotation = FRotator(0, 30, 0);
            }
            else if (isTopRow) { // 북동 코너 (Top-Right) - 겹침 발생
                WallRotation = FRotator(0, 90, 0);
            }
            break;
    }

    return WallRotation;
}

bool ADungeonGenerator::DoesRoomOverlap(const FIntRect& ProposedRoomRect, const TArray<FIntRect>& ExistingRoomRects)
{
    for (const FIntRect& ExistingRect : ExistingRoomRects)
    {
        if (ProposedRoomRect.Intersect(ExistingRect))
        {
            return true;
        }
    }
    return false;
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
