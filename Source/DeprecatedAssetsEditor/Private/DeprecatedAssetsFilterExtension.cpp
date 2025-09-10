// Copyright Boneshock Studio AS 2024.

#include "DeprecatedAssetsFilterExtension.h"
#include "DeprecatedAssetFrontendFilter.h"

#define LOCTEXT_NAMESPACE "UDeprecatedAssetsFilterExtension"

void UDeprecatedAssetsFilterExtension::AddFrontEndFilterExtensions(TSharedPtr<FFrontendFilterCategory> DefaultCategory,TArray<TSharedRef<FFrontendFilter>>& InOutFilterList) const
{
	
	TSharedRef<FFrontendFilterCategory> DeprecatedCategory = MakeShared<FFrontendFilterCategory>(
		LOCTEXT("DeprecatedAssetsCategory", "Deprecated Assets"),
		LOCTEXT("DeprecatedAssetsCategoryTooltip", "WIP DO NOT USE FOR NOW AS IT FETCH ASSET IN A VERY SLOW MANNER // Filters assets marked as deprecated")
	);

	
	InOutFilterList.Add(MakeShared<FFrontendFilter_DeprecatedAssets>(DeprecatedCategory));
}

#undef LOCTEXT_NAMESPACE
