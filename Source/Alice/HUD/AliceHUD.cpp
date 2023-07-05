// Fill out your copyright notice in the Description page of Project Settings.


#include "AliceHUD.h"

void AAliceHUD::DrawHUD()
{
	Super::DrawHUD();

	if (CrosshairCenter)
	{
		if (GEngine)
		{
			FVector2D ViewportSize;
			GEngine->GameViewport->GetViewportSize(ViewportSize);
			const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

			const float TextureWidth = CrosshairCenter->GetSizeX();
			const float TextureHeight = CrosshairCenter->GetSizeY();
			const FVector2D TextureDrawPoint(
				ViewportCenter.X - (TextureWidth / 2.f),
				ViewportCenter.Y - (TextureHeight / 2.f)
			);

			DrawTexture(
				CrosshairCenter,
				TextureDrawPoint.X,
				TextureDrawPoint.Y,
				TextureWidth,
				TextureHeight,
				0.f,
				0.f,
				1.f,
				1.f,
				FColor::White
			);
		}
	}
}

void AAliceHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UAliceCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void AAliceHUD::AddTaggerOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && TaggerOverlayClass)
	{
		TaggerOverlay = CreateWidget<UUserWidget>(PlayerController, TaggerOverlayClass);
		TaggerOverlay->AddToViewport();
	}
}
