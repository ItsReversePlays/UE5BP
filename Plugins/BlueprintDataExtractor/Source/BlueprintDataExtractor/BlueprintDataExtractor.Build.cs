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
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Projects", // Needed for accessing project paths
                "Json",     // For serializing data to JSON
                "JsonUtilities"
            }
        );
    }
}
