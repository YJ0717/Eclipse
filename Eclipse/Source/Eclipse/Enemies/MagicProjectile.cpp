// Fill out your copyright notice in the Description page of Project Settings.

#include "MagicProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

AMagicProjectile::AMagicProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// Sphere 충돌 컴포넌트 생성 (루트)
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(30.0f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	RootComponent = CollisionComp;

	// Niagara 컴포넌트 생성
	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComp"));
	NiagaraComp->SetupAttachment(RootComponent);

	// Projectile Movement 컴포넌트
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 2000.0f;
	ProjectileMovement->MaxSpeed = 2000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f; // 중력 없음

	// 5초 후 자동 소멸
	InitialLifeSpan = 5.0f;
}

void AMagicProjectile::BeginPlay()
{
	Super::BeginPlay();

	// OnHit 이벤트 바인딩
	if (CollisionComp)
	{
		CollisionComp->OnComponentHit.AddDynamic(this, &AMagicProjectile::OnHit);
		CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AMagicProjectile::OnOverlap);
	}

	UE_LOG(LogTemp, Warning, TEXT("MagicProjectile spawned at %s"), *GetActorLocation().ToString());
}

void AMagicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMagicProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("MagicProjectile OnHit called! OtherActor: %s"), 
		OtherActor ? *OtherActor->GetName() : TEXT("NULL"));

	// 자기 자신이나 소유자(보스)는 무시
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("MagicProjectile: Ignoring hit (self or owner)"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("MagicProjectile hit valid target: %s"), *OtherActor->GetName());

	// 플레이어에게 데미지
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("MagicProjectile: Applying %.1f damage to player!"), Damage);
		UGameplayStatics::ApplyDamage(Player, Damage, GetInstigatorController(), GetOwner(), UDamageType::StaticClass());
		UE_LOG(LogTemp, Warning, TEXT("MagicProjectile: Damage applied successfully!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MagicProjectile: Hit actor is not a player!"));
	}

	// 충돌 후 파괴
	Destroy();
}

void AMagicProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("MagicProjectile OnOverlap called! OtherActor: %s"), 
		OtherActor ? *OtherActor->GetName() : TEXT("NULL"));

	// 자기 자신이나 소유자(보스)는 무시
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("MagicProjectile Overlap: Ignoring (self or owner)"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("MagicProjectile overlap valid target: %s"), *OtherActor->GetName());

	// 플레이어에게 데미지
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("MagicProjectile Overlap: Applying %.1f damage to player!"), Damage);
		UGameplayStatics::ApplyDamage(Player, Damage, GetInstigatorController(), GetOwner(), UDamageType::StaticClass());
		UE_LOG(LogTemp, Warning, TEXT("MagicProjectile Overlap: Damage applied!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MagicProjectile Overlap: Not a player!"));
	}

	// 충돌 후 파괴
	Destroy();
}
