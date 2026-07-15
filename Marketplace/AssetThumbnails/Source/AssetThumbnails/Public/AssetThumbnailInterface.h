// Copyright 2024 Creative Elicya. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AssetThumbnailInterface.generated.h"

UENUM(BlueprintType)
enum class EAssetThumbnailTextPosition : uint8
{
	TopLeft UMETA(DisplayName = "Top Left"),
	TopCenter UMETA(DisplayName = "Top Center"),
	TopRight UMETA(DisplayName = "Top Right"),

	MiddleLeft UMETA(DisplayName = "Middle Left"),
	Center UMETA(DisplayName = "Center"),
	MiddleRight UMETA(DisplayName = "Middle Right"),

	BottomLeft UMETA(DisplayName = "Bottom Left"),
	BottomCenter UMETA(DisplayName = "Bottom Center"),
	BottomRight UMETA(DisplayName = "Bottom Right"),
};

UINTERFACE(MinimalAPI)
class UAssetThumbnailInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Asset Thumbnail Interface
 */
class ASSETTHUMBNAILS_API IAssetThumbnailInterface
{
	GENERATED_BODY()

public:
	// Replace the Asset thumbnail
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Asset Thumbnail")
	UTexture2D *GetThumbnailIcon() const;

	// Set the Asset thumbnail title
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Asset Thumbnail")
	FString GetThumbnailTitle() const;

	// Set the Asset thumbnail title position
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Asset Thumbnail")
	EAssetThumbnailTextPosition GetThumbnailTitlePosition() const;

	// Set the Asset thumbnail border color
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Asset Thumbnail")
	FLinearColor GetThumbnailBorderColor() const;

	// Set the Asset thumbnail border width
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Asset Thumbnail")
	float GetThumbnailBorderWidth() const;
};
