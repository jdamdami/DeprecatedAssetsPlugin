#pragma once

#include "CoreMinimal.h"

class FDeprecatedContentBrowserExtender
{
public:
	
	static FDelegateHandle Register();
	
	static void Unregister(FDelegateHandle Handle);

private:
	
	static TSharedRef<FExtender> OnExtend(const TArray<FAssetData>& SelectedAssets);

	static void BuildMenu(FMenuBuilder& MenuBuilder, TArray<FAssetData> SelectedAssets);

	static void MarkAssetAsDeprecated(TArray<FAssetData> SelectedAssets);

	static void SetAssetReplacement(TArray<FAssetData> SelectedAssets);

	static void UnmarkAssetAsDeprecated(TArray<FAssetData> SelectedAssets);

	static void ShowPickReplacementDialog(TFunction<void(UObject*)> OnPicked,UObject* InitialReplacement = nullptr);

	static void Notify(const FText& Message, bool bSuccess);
};
