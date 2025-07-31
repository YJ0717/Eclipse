// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h" 
#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class ECLIPSE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// 이 캐릭터의 속성에 대한 기본값을 설정합니다.
	APlayerCharacter();

protected:
	// 게임이 시작되거나 스폰될 때 호출됩니다.
	virtual void BeginPlay() override;
	// 입력을 기능에 바인딩하기 위해 호출됩니다.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 이동 입력을 위해 호출됩니다.
	void Move(const FInputActionValue& Value);

	// 둘러보기 입력을 위해 호출됩니다.
	void Look(const FInputActionValue& Value);

	// 공격 입력을 위해 호출됩니다.
	void Attack(const FInputActionValue& Value);

public:	
	// 매 프레임 호출됩니다.
	virtual void Tick(float DeltaTime) override;

protected:
	// 입력 매핑 컨텍스트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	// 점프 입력 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	// 이동 입력 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	// 둘러보기 입력 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	// 무기 장착 입력 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipAction;

	// 공격 입력 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	// 캐릭터 뒤에 카메라를 배치하는 카메라 붐
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	// 따라가는 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	// 도끼 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* AxeComponent;

	// 무기 장착 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bIsWeaponEquipped;

	// 무기 장착/해제 함수
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void ToggleWeapon();

	// 무기 장착 애니메이션 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* EquipMontage;

	// 무기 해제 애니메이션 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* UnequipMontage;

	// 공격 애니메이션 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* AttackMontage;

	// 벽 충돌 시 생성될 파티클 이펙트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Effects, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactEffect;

	// 이전 프레임의 도끼 위치
	FVector PreviousAxeLocation;

	// 무기 해제 완료 타이머 핸들
	FTimerHandle UnequipTimerHandle;

	// 무기 해제 완료 시 호출될 함수
	void FinishUnequip();
};

