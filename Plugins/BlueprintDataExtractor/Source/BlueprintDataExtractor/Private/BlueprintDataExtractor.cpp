#include "BlueprintDataExtractor.h"

#define LOCTEXT_NAMESPACE "FBlueprintDataExtractorModule"

void FBlueprintDataExtractorModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing depends on the LoadingPhase in your .uplugin file
}

void FBlueprintDataExtractorModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBlueprintDataExtractorModule, BlueprintDataExtractor)
