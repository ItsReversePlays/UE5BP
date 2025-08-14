#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BPE_ExtractorFunctionLibrary.generated.h"

UCLASS()
class BLUEPRINTDATAEXTRACTOR_API UBPE_ExtractorFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Extracts information about all blueprint-callable functions and saves it to a JSON file.
     * @param FilePath The full path where the JSON file will be saved. If empty, a default path in the project's Saved directory will be used.
     * @return True if the export was successful, false otherwise.
     */
    UFUNCTION(BlueprintCallable, Category = "Development|Blueprint Data Extractor")
    static bool ExportBlueprintFunctionData(FString FilePath = "");
};
