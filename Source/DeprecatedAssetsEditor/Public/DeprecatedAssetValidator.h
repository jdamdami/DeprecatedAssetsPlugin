#pragma once

#include "CoreMinimal.h"
#include "EditorValidatorBase.h"
#include "DeprecatedAssetValidator.generated.h"

/**
 * Data Validation: fails when an asset is marked deprecated; includes the suggested replacement.
 */

UCLASS(DisplayName="Deprecated Assets Validator")
class DEPRECATEDASSETSEDITOR_API UDeprecatedAssetValidator : public UEditorValidatorBase
{
	GENERATED_BODY()

public:

	virtual bool CanValidateAsset_Implementation(const FAssetData& InAssetData, UObject* InObject, FDataValidationContext& InContext) const override;
	
	virtual EDataValidationResult ValidateLoadedAsset_Implementation(const FAssetData& InAssetData, UObject* InAsset, FDataValidationContext& Context) override;


	
};
