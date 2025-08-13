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
	
	virtual bool CanValidateAsset_Implementation(UObject* InAsset) const override;

	virtual EDataValidationResult ValidateLoadedAsset_Implementation(UObject* InAsset, TArray<FText>& ValidationErrors) override;
};
