#include "DeprecatedAssetValidator.h"
#include "DeprecatedAssetMetadata.h"
#include "DeprecatedAssetsEditor.h"
#include "DeprecatedAssetSettings.h"
#include "Misc/DataValidation.h"



bool UDeprecatedAssetValidator::CanValidateAsset_Implementation(const FAssetData& InAssetData, UObject* InObject,FDataValidationContext& InContext) const
{
	return InObject != nullptr;
}

EDataValidationResult UDeprecatedAssetValidator::ValidateLoadedAsset_Implementation(const FAssetData& InAssetData,UObject* InAsset, FDataValidationContext& Context)
{
	if (!InAsset)
	{
		return EDataValidationResult::NotValidated;
	}

	const UDeprecatedAssetSettings* Settings = GetDefault<UDeprecatedAssetSettings>();

	if (!Settings->bEnableFullErrorReport)
	{
		//UE_LOG(LogDeprecatedAssetPlugin,Error,TEXT("USE OF DEPRECATED ASSET WITH ERROR REPORT DEACTIVATED, REACTIVATE ERROR REPORT"));

		return EDataValidationResult::Valid;
	}

	FDeprecatedAssetInfo Info;
	
	UDeprecatedAssetMetadataLibrary::GetDeprecatedInfo(InAsset, Info);

	if (Info.bIsDeprecated)
	{
		const FString ReplacementStr = Info.Replacement.IsNull()
			? TEXT("<none specified>")
			: Info.Replacement.ToSoftObjectPath().ToString();

		Context.AddError(FText::Format(
			NSLOCTEXT("DeprecatedAssetValidator", "AssetDeprecatedMsg",
				"Asset '{0}' is deprecated. Use '{1}' instead."),
			FText::FromString(InAsset->GetPathName()),
			FText::FromString(ReplacementStr)
		));

		

		return EDataValidationResult::Invalid;
	}

	return EDataValidationResult::Valid;
}



