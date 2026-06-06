// Copyright Guilty Saint Games 2026, All Rights Reserved.

using UnrealBuildTool;

public class BlendPosesByTagEditor: ModuleRules
{
    public BlendPosesByTagEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "BlendPosesByTag"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "AnimGraph",
                "BlueprintGraph",
                "AnimGraphRuntime",
                "GameplayTags",
                "UnrealEd",
                "ToolMenus",
                "GameplayTagsEditor"
            }
        );
    }
}
