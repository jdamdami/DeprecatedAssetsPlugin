#pragma once

#include "CoreMinimal.h"
#include "FrontendFilterBase.h"
#include "ContentBrowserItemData.h"
#include "DeprecatedAssetMetadata.h"


class FFrontendFilter_DeprecatedAssets final : public FFrontendFilter
{
	
public:
	
	FFrontendFilter_DeprecatedAssets(TSharedPtr<FFrontendFilterCategory> InCategory): FFrontendFilter(InCategory) {}

	virtual FString GetName() const override { return TEXT("DeprecatedAssetsFilter"); }

	virtual FText GetDisplayName() const override
	{
		return NSLOCTEXT("DeprecatedAssetsFilter", "DisplayName", "Has Deprecated Meta");
	}

	virtual FText GetToolTipText() const override
	{
		return NSLOCTEXT("DeprecatedAssetsFilter", "Tooltip", "WIP DO NOT USE FOR NOW AS IT FETCH ASSET IN A VERY SLOW MANNER // Filters assets with the DeprecatedAssets.Deprecated metadata key.");
	}

	virtual bool PassesFilter(FAssetFilterType InItem) const override
	{
		bool bIsDeprecated = false;

		FAssetData AssetData;
		
		if (InItem.Legacy_TryGetAssetData(AssetData))
		{
			
			if (UPackage* Package = AssetData.GetPackage())
			{
				FMetaData& Meta = Package->GetMetaData();
				
				const FString Value = Meta.GetValue(AssetData.GetAsset(), UDeprecatedAssetMetadataLibrary::MetaKey_Deprecated);

				if (Value.Equals(TEXT("True"), ESearchCase::IgnoreCase) || Value == TEXT("1"))
				{
					bIsDeprecated = true;
				}
			}
		}

		return bIsDeprecated;
	}
	
};
