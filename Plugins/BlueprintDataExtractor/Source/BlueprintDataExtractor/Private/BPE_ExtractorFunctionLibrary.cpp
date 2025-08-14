#include "BPE_ExtractorFunctionLibrary.h"
#include "UObject/UObjectIterator.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"
#include "JsonObjectConverter.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

// Helper function to get a string representation of a property's type
FString GetPropertyType(FProperty* Property)
{
    if (!Property) return TEXT("Invalid");

    if (FNumericProperty *NumericProperty = CastField<FNumericProperty>(Property))
    {
        if (NumericProperty->IsInteger()) return TEXT("int");
        if (NumericProperty->IsFloatingPoint()) return TEXT("float");
        return TEXT("numeric");
    }
    if (FBoolProperty *BoolProperty = CastField<FBoolProperty>(Property)) return TEXT("bool");
    if (FNameProperty *NameProperty = CastField<FNameProperty>(Property)) return TEXT("Name");
    if (FStrProperty *StringProperty = CastField<FStrProperty>(Property)) return TEXT("string");
    if (FTextProperty *TextProperty = CastField<FTextProperty>(Property)) return TEXT("Text");
    if (FObjectProperty *ObjectProperty = CastField<FObjectProperty>(Property))
    {
        return FString::Printf(TEXT("Object (%s)"), *ObjectProperty->PropertyClass->GetName());
    }
    if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
    {
        return FString::Printf(TEXT("Struct (%s)"), *StructProperty->Struct->GetName());
    }
    if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
    {
        return FString::Printf(TEXT("Enum (%s)"), *EnumProperty->GetEnum()->GetName());
    }
    if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
    {
        return FString::Printf(TEXT("Array<%s>"), *GetPropertyType(ArrayProperty->Inner));
    }
    if (FMapProperty* MapProperty = CastField<FMapProperty>(Property))
    {
        return FString::Printf(TEXT("Map<%s, %s>"), *GetPropertyType(MapProperty->KeyProp), *GetPropertyType(MapProperty->ValueProp));
    }

    return Property->GetClass()->GetName();
}

bool UBPE_ExtractorFunctionLibrary::ExportBlueprintFunctionData(FString FilePath)
{
    FString FinalFilePath = FilePath;
    if (FinalFilePath.IsEmpty())
    {
        FinalFilePath = FPaths::ProjectSavedDir() / TEXT("BlueprintData.json");
    }

    TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject());
    TSharedPtr<FJsonValueArray> AllFunctionsArray = MakeShareable(new FJsonValueArray());

    // Iterate over all loaded UClasses
    for (TObjectIterator<UClass> It; It; ++It)
    {
        UClass* CurrentClass = *It;

        // Iterate over all functions in the current class
        for (TFieldIterator<UFunction> FuncIt(CurrentClass, EFieldIteratorFlags::ExcludeSuper); FuncIt; ++FuncIt)
        {
            UFunction* Function = *FuncIt;

            // Check if the function is blueprint callable
            if (Function->HasAnyFunctionFlags(EFunctionFlags::BlueprintCallable))
            {
                TSharedPtr<FJsonObject> FunctionObject = MakeShareable(new FJsonObject());
                FunctionObject->SetStringField(TEXT("ClassName"), CurrentClass->GetName());
                FunctionObject->SetStringField(TEXT("FunctionName"), Function->GetName());
                FunctionObject->SetStringField(TEXT("Category"), Function->GetMetaData(TEXT("Category")));
                FunctionObject->SetBoolField(TEXT("IsPure"), Function->HasAnyFunctionFlags(EFunctionFlags::BlueprintPure));

                TSharedPtr<FJsonValueArray> ParamsArray = MakeShareable(new FJsonValueArray());

                // Iterate over all properties (parameters) of the function
                for (TFieldIterator<FProperty> PropIt(Function); PropIt; ++PropIt)
                {
                    FProperty* Property = *PropIt;
                    TSharedPtr<FJsonObject> ParamObject = MakeShareable(new FJsonObject());

                    FString Direction = TEXT("In");
                    if (Property->HasAnyPropertyFlags(EPropertyFlags::ReturnParm))
                    {
                        Direction = TEXT("Return");
                    }
                    else if (Property->HasAnyPropertyFlags(EPropertyFlags::OutParm))
                    {
                        Direction = TEXT("Out");
                    }

                    ParamObject->SetStringField(TEXT("Name"), Property->GetName());
                    ParamObject->SetStringField(TEXT("Type"), GetPropertyType(Property));
                    ParamObject->SetStringField(TEXT("Direction"), Direction);

                    ParamsArray->Array.Add(MakeShareable(new FJsonValueObject(ParamObject)));
                }

                FunctionObject->SetArrayField(TEXT("Parameters"), ParamsArray->Array);
                AllFunctionsArray->Array.Add(MakeShareable(new FJsonValueObject(FunctionObject)));
            }
        }
    }

    RootObject->SetArrayField(TEXT("BlueprintCallableFunctions"), AllFunctionsArray->Array);

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);

    return FFileHelper::SaveStringToFile(OutputString, *FinalFilePath);
}
