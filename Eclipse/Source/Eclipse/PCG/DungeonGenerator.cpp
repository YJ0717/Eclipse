
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
    // 최소 2개의 복도가 생성되도록 보장 (NumRooms >= 3)
    if (NumRooms < 3)
    {
        NumRooms = 3;
        UE_LOG(LogTemp, Warning, TEXT("NumRooms was less than 3, setting to 3 to ensure at least 2 corridors."));
    }

    // 생성 전 이전 던전 정리
    ClearDungeon();
    OccupiedRoomAreas.Empty(); // 방 영역 초기화
    OccupiedWallLocations.Empty(); // 벽 위치 초기화

    if (!FloorMesh || !WallMesh || !CorridorMesh)
    {
        //UE_LOG(LogTemp, Warning, TEXT("Dungeon meshes are not set!"));
        return;
    }

    FVector CurrentLocation = GetActorLocation();
    TArray<FVector> RoomCenters; // 이 배열은 더 이상 충돌 감지에 사용되지 않습니다.
    
    // 첫 번째 방 생성
    FIntPoint InitialRoomSize;
    InitialRoomSize.X = FMath::RandRange(RoomSizeMin.X / 2, RoomSizeMax.X / 2) * 2;
    InitialRoomSize.Y = FMath::RandRange(RoomSizeMin.Y / 2, RoomSizeMax.Y / 2) * 2;

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
            ProposedRoomSize.X = FMath::RandRange(RoomSizeMin.X / 2, RoomSizeMax.X / 2) * 2;
            ProposedRoomSize.Y = FMath::RandRange(RoomSizeMin.Y / 2, RoomSizeMax.Y / 2) * 2;

            // 다음 방으로 이동하기 위한 복도 생성 방향
            FVector Direction;
            if (i % 2 != 0) // 홀수 번째 방은 수평으로 복도 생성
            {
                Direction = (FMath::RandBool()) ? FVector::RightVector : FVector::LeftVector; // East or West
            }
            else // 짝수 번째 방은 수직으로 복도 생성
            {
                Direction = (FMath::RandBool()) ? FVector::ForwardVector : FVector::BackwardVector; // North or South
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

            // 복도 생성
            FVector CorridorStartPoint = CurrentLocation; // 이전 방의 중심
            FVector CorridorEndPoint = ProposedNextRoomLocation; // 새로 배치된 방의 중심
            SpawnCorridor(CorridorStartPoint, CorridorEndPoint);
            
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
    FVector BaseTileWorldCenter = Center + FirstTileCenterOffset;

    // Calculate the base location for floor tiles, including the offset
    FVector FloorBaseLocation = BaseTileWorldCenter + FloorOffset;

    // Calculate the base location for wall tiles, including the offset
    FVector WallBaseLocation = BaseTileWorldCenter + WallOffset;

    // Spawn Floor Tiles
    for (int32 x = 0; x < Size.X; ++x)
    {
        for (int32 y = 0; y < Size.Y; ++y)
        {
            FVector FloorTileLocation = FloorBaseLocation + FVector(x * TileSize, y * TileSize, 0);
            SpawnMesh(FloorMesh, FloorTileLocation, FRotator::ZeroRotator, FloorScale);
        }
    }

            // 벽 생성 (가장자리)
            // X축을 따라 벽 생성 (상단 및 하단)
            for (int32 WallX = 0; WallX < Size.X; ++WallX)
            {
                // 하단 벽
                FVector BottomWallLocation = WallBaseLocation + FVector(WallX * TileSize, -0.5f * TileSize, 0);
                FRotator BottomWallRotation = GetWallRotationForLocation(WallX, 0, Size, EWallType::Bottom);
                AActor* SpawnedWallActor = SpawnMesh(WallMesh, BottomWallLocation, BottomWallRotation);
                OccupiedWallLocations.Add(WorldToGrid(BottomWallLocation - FVector(0, 0.01f, 0)), SpawnedWallActor);
                UE_LOG(LogTemp, Warning, TEXT("SpawnRoom: Added wall at grid %d, %d"), WorldToGrid(BottomWallLocation - FVector(0, 0.01f, 0)).X, WorldToGrid(BottomWallLocation - FVector(0, 0.01f, 0)).Y);

                // 상단 벽
                FVector TopWallLocation = WallBaseLocation + FVector(WallX * TileSize, (Size.Y - 0.5f) * TileSize, 0);
                FRotator TopWallRotation = GetWallRotationForLocation(WallX, Size.Y - 1, Size, EWallType::Top);
                SpawnedWallActor = SpawnMesh(WallMesh, TopWallLocation, TopWallRotation);
                OccupiedWallLocations.Add(WorldToGrid(TopWallLocation + FVector(0, 0.01f, 0)), SpawnedWallActor);
                UE_LOG(LogTemp, Warning, TEXT("SpawnRoom: Added wall at grid %d, %d"), WorldToGrid(TopWallLocation + FVector(0, 0.01f, 0)).X, WorldToGrid(TopWallLocation + FVector(0, 0.01f, 0)).Y);
            }

            // Y축을 따라 벽 생성 (왼쪽 및 오른쪽) - 코너 포함 (겹침 발생)
            for (int32 WallY = 0; WallY < Size.Y; ++WallY) // WallY = 0 (하단) 및 WallY = Size.Y - 1 (상단)도 포함
            {
                // 왼쪽 벽
                FVector LeftWallLocation = WallBaseLocation + FVector(-0.5f * TileSize, WallY * TileSize, 0);
                FRotator LeftWallRotation = GetWallRotationForLocation(0, WallY, Size, EWallType::Left);
                AActor* SpawnedWallActor = SpawnMesh(WallMesh, LeftWallLocation, LeftWallRotation);
                OccupiedWallLocations.Add(WorldToGrid(LeftWallLocation - FVector(0.01f, 0, 0)), SpawnedWallActor);
                UE_LOG(LogTemp, Warning, TEXT("SpawnRoom: Added wall at grid %d, %d"), WorldToGrid(LeftWallLocation - FVector(0.01f, 0, 0)).X, WorldToGrid(LeftWallLocation - FVector(0.01f, 0, 0)).Y);

                // 오른쪽 벽
                FVector RightWallLocation = WallBaseLocation + FVector((Size.X - 0.5f) * TileSize, WallY * TileSize, 0);
                FRotator RightWallRotation = GetWallRotationForLocation(Size.X - 1, WallY, Size, EWallType::Right);
                SpawnedWallActor = SpawnMesh(WallMesh, RightWallLocation, RightWallRotation);
                OccupiedWallLocations.Add(WorldToGrid(RightWallLocation + FVector(0.01f, 0, 0)), SpawnedWallActor);
                UE_LOG(LogTemp, Warning, TEXT("SpawnRoom: Added wall at grid %d, %d"), WorldToGrid(RightWallLocation + FVector(0.01f, 0, 0)).X, WorldToGrid(RightWallLocation + FVector(0.01f, 0, 0)).Y);
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

FIntPoint ADungeonGenerator::WorldToGrid(const FVector& WorldLocation)
{
    return FIntPoint(FMath::FloorToInt(WorldLocation.X / TileSize), FMath::FloorToInt(WorldLocation.Y / TileSize));
}

void ADungeonGenerator::SpawnCorridor(const FVector& Start, const FVector& End)
{
    FVector Direction = (End - Start).GetSafeNormal();
    float Distance = FVector::Dist(Start, End);
    int32 NumTiles = FMath::CeilToInt(Distance / TileSize);
    FIntPoint EntrancePoint(0,0);
    bool bEntranceWallFound = false;
    bool bExitWallDestroyed = false;

    for (int32 i = 0; i < NumTiles; ++i)
    {
        FVector Location = Start + Direction * i * TileSize;
        FIntPoint GridLocation = WorldToGrid(Location);

        if (OccupiedWallLocations.Contains(GridLocation))
        {
            if (!bExitWallDestroyed)
            {
                // 첫 번째로 만나는 벽은 출발 지점의 '출구 벽'입니다. 이 벽은 파괴합니다.
                AActor* WallActor = OccupiedWallLocations[GridLocation];
                if (WallActor) { WallActor->Destroy(); }
                OccupiedWallLocations.Remove(GridLocation);
                bExitWallDestroyed = true;
            }
            else
            {
                // 두 번째 이후로 만나는 벽은 도착 지점의 '입구 벽'입니다.
                // 이 벽은 파괴하지 않고, 위치만 기록합니다.
                EntrancePoint = GridLocation;
                bEntranceWallFound = true;
            }
        }

        SpawnMesh(CorridorMesh, Location, Direction.Rotation());
    }

    // 입구 벽 위치를 기준으로 왼쪽 벽을 찾아 파괴합니다.
    if (bEntranceWallFound)
    {
        FVector LeftVector = FVector::CrossProduct(Direction, FVector::UpVector);
        FIntPoint LeftGridOffset = FIntPoint(FMath::RoundToInt(LeftVector.X), FMath::RoundToInt(LeftVector.Y));
        FIntPoint LeftGridLocation = EntrancePoint + LeftGridOffset;

        if (OccupiedWallLocations.Contains(LeftGridLocation))
        {
            AActor* WallActor = OccupiedWallLocations[LeftGridLocation];
            if (WallActor)
            {
                WallActor->Destroy();
            }
            OccupiedWallLocations.Remove(LeftGridLocation);
        }
    }
}

AActor* ADungeonGenerator::SpawnMesh(UStaticMesh* Mesh, const FVector& Location, const FRotator& Rotation, const FVector& Scale)
{
    if (UWorld* World = GetWorld())
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        AStaticMeshActor* MeshActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, Rotation, SpawnParams);
        if (MeshActor)
        {
            MeshActor->GetStaticMeshComponent()->SetStaticMesh(Mesh);
            MeshActor->SetActorScale3D(Scale);
            SpawnedActors.Add(MeshActor);
            return MeshActor; // Return the spawned actor
        }
    }
    return nullptr; // Return nullptr if spawning fails
}
