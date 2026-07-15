// Copyright 2024 Creative Elicya. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "AssetThumbnailInterface.h"
#include "ThumbnailRendering/BlueprintThumbnailRenderer.h"
#include "CustomBlueprintThumbnailRenderer.generated.h"

// class FCanvas;
// class FRenderTarget;

/**
 * Custom BlueprintThumbnailRenderer
 */
UCLASS()
class UCustomBlueprintThumbnailRenderer : public UBlueprintThumbnailRenderer
{
	GENERATED_BODY()

	// Thumbnail Renderer GetThumbnailSize method override.
	virtual void GetThumbnailSize(UObject *Object, float Zoom, uint32 &OutWidth, uint32 &OutHeight) const override;
	// Thumbnail Renderer Draw method override.
	virtual void Draw(UObject *Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget *RenderTarget, FCanvas *Canvas, bool bAdditionalViewFamily) override;
	// Thumbnail Renderer CanVisualizeAsset method override.
	virtual bool CanVisualizeAsset(UObject *Object) override;

protected:
	// Get the Asset Object
	UObject *GetAssetObject(UObject *Object) const;
};
