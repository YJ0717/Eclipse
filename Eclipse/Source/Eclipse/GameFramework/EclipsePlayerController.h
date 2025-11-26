// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EclipsePlayerController.generated.h"

class UInputMappingContext;

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class AEclipsePlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="Input", meta = (AllowPrivateAccess = "true"))
	TArray<UInputMappingContext*> DefaultMappingContexts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* PauseAction; // 일시정지 액션

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> PauseWidgetClass; // 일시정지 UI 위젯 클래스

protected:

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override; // BeginPlay 선언 추가

		UFUNCTION(BlueprintCallable)
	void TogglePauseGame(); // 게임 일시정지/재개 함수

private:
	UPROPERTY()
	class UUserWidget* PauseWidgetInstance; // 생성된 일시정지 UI 위젯 인스턴스

}; // 클래스 정의 끝에 세미콜론 추가
