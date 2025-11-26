// Copyright Epic Games, Inc. All Rights Reserved.


#include "EclipsePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h" // UGameplayStatics 사용을 위해 추가
#include "EnhancedInputComponent.h" // 입력 바인딩을 위해 추가
#include "Blueprint/UserWidget.h" // UUserWidget 사용을 위해 추가

void AEclipsePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Input Mapping Contexts are added in SetupInputComponent
}

void AEclipsePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Add Input Mapping Contexts
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
		{
			Subsystem->AddMappingContext(CurrentContext, 0);
		}
	}

	// Bind Pause Action
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (PauseAction)
		{
			EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Triggered, this, &AEclipsePlayerController::TogglePauseGame);
		}
	}
}

void AEclipsePlayerController::TogglePauseGame()
{
	// 디버그 메시지: TogglePauseGame 호출됨
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("TogglePauseGame Called!"));
	}

	bool bIsGamePaused = UGameplayStatics::IsGamePaused(GetWorld());
	UGameplayStatics::SetGamePaused(GetWorld(), !bIsGamePaused);

	if (!bIsGamePaused) // 게임이 일시정지될 때
	{
		// UI 생성 및 표시
		if (PauseWidgetClass && !PauseWidgetInstance)
		{
			PauseWidgetInstance = CreateWidget<UUserWidget>(this, PauseWidgetClass);
			// 디버그 메시지: 위젯 생성 시도
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, FString::Printf(TEXT("Creating Widget. Class: %s"), *PauseWidgetClass->GetName()));
			}
		}

		if (PauseWidgetInstance)
		{
			PauseWidgetInstance->AddToViewport();
			// 디버그 메시지: 위젯 뷰포트에 추가됨
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Widget Added to Viewport!"));
			}
		}

		SetInputMode(FInputModeUIOnly()); // UI만 입력 받도록 설정
		bShowMouseCursor = true; // 마우스 커서 표시
	}
	else // 게임이 재개될 때
	{
		// UI 숨김
		if (PauseWidgetInstance)
		{
			PauseWidgetInstance->RemoveFromParent();
			// 디버그 메시지: 위젯 뷰포트에서 제거됨
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Widget Removed from Viewport!"));
			}
		}

		SetInputMode(FInputModeGameOnly()); // 게임만 입력 받도록 설정
		bShowMouseCursor = false; // 마우스 커서 숨김
	}
}
