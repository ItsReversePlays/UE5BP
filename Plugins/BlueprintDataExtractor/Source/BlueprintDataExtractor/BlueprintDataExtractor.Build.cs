using UnrealBuildTool;

public class BlueprintDataExtractor : ModuleRules
{
    public BlueprintDataExtractor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "Json",
                "JsonUtilities"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Projects", // Needed for accessing project paths
            }
        );
    }
}
