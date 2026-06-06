// Copyright 2024 Creative Elicya. All Rights Reserved.

#include "AssetThumbnailsEditor.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "CustomBlueprintThumbnailRenderer.h"

#define LOCTEXT_NAMESPACE "FAssetThumbnailsEditorModule"

void FAssetThumbnailsEditorModule::StartupModule()
{
	// Unregister default Blueprint thumbnail renderer
	UThumbnailManager::Get().UnregisterCustomRenderer(UBlueprint::StaticClass());
	// Register a custom Blueprint thumbnail renderer
	UThumbnailManager::Get().RegisterCustomRenderer(UBlueprint::StaticClass(), UCustomBlueprintThumbnailRenderer::StaticClass());

	// Unregister default DataAsset thumbnail renderer
	UThumbnailManager::Get().UnregisterCustomRenderer(UDataAsset::StaticClass());
	// Register a custom DataAsset thumbnail renderer
	UThumbnailManager::Get().RegisterCustomRenderer(UDataAsset::StaticClass(), UCustomBlueprintThumbnailRenderer::StaticClass());
}

void FAssetThumbnailsEditorModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAssetThumbnailsEditorModule, AssetThumbnailsEditor)