using UnrealBuildTool;

public class FPAnimGraph : ModuleRules
{
    public FPAnimGraph(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "AnimationModifiers", 
                "AnimationBlueprintLibrary",
                "BlueprintGraph",
                "FPFramework",
                "AnimGraphRuntime",
                "AnimGraph",
                "GraphEditor"
            }
        );
    }
}