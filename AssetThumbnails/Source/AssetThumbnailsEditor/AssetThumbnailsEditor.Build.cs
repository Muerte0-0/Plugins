// Copyright 2024 Creative Elicya. All Rights Reserved.

using UnrealBuildTool;

public class AssetThumbnailsEditor : ModuleRules
{
	public AssetThumbnailsEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "AssetThumbnails",
            }
			);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "UnrealEd",
            }
			);
	}
}
