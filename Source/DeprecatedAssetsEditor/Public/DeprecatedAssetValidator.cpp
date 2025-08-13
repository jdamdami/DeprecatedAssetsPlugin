#include "DeprecatedAssetValidator.h"
#include "DeprecatedAssetMetadata.h"

bool UDeprecatedAssetValidator::CanValidateAsset_Implementation(UObject* InAsset) const
{
	return InAsset != nullptr;
}

EDataValidationResult UDeprecatedAssetValidator::ValidateLoadedAsset_Implementation(UObject* InAsset, TArray<FText>& ValidationErrors)
{
	if (!InAsset)
	{
		return EDataValidationResult::NotValidated;
	}

	FDeprecatedAssetInfo Info;
	
	UDeprecatedAssetMetadataLibrary::GetDeprecatedInfo(InAsset, Info);

	if (Info.bIsDeprecated)
	{
		const FString ReplacementStr = Info.Replacement.IsNull()
			? TEXT("<none specified>")
			: Info.Replacement.ToSoftObjectPath().ToString();

		ValidationErrors.Add(
			FText::Format(
				NSLOCTEXT("DeprecatedAssetValidator", "AssetDeprecatedMsg",
					"Asset '{0}' is deprecated. Use '{1}' instead."),
				FText::FromString(InAsset->GetPathName()),
				FText::FromString(ReplacementStr)
			)
		);

		return EDataValidationResult::Invalid;
	}

	return EDataValidationResult::Valid;
}
