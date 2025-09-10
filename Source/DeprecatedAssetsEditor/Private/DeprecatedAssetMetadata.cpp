#include "DeprecatedAssetMetadata.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/Package.h"



const FName UDeprecatedAssetMetadataLibrary::MetaKey_Deprecated(TEXT("DeprecatedAssets.Deprecated"));
const FName UDeprecatedAssetMetadataLibrary::MetaKey_Replacement(TEXT("DeprecatedAssets.Replacement"));


FMetaData* UDeprecatedAssetMetadataLibrary::GetMetaForAsset(UObject* Asset)
{
	if (!Asset) return nullptr;

	if (UPackage* Package = Asset->GetOutermost())
	{
		return &Package->GetMetaData(); 
	}

	return nullptr;
}

FString UDeprecatedAssetMetadataLibrary::MakeCompositeKey(UObject* Asset, const FName& Key)
{
	return Asset->GetName() + TEXT(".") + Key.ToString();
}

bool UDeprecatedAssetMetadataLibrary::SetMetaValue(UObject* Asset, const FName& Key, const FString& Value)
{
	if (!Asset) return false;

	if (FMetaData* MD = GetMetaForAsset(Asset))
	{
		MD->SetValue(Asset, Key, *Value);
		
		if (UPackage* Package = Asset->GetOutermost())
		{
			Package->SetDirtyFlag(true);
		}
		
		return true;
	}
	
	return false;
}

bool UDeprecatedAssetMetadataLibrary::RemoveMetaValue(UObject* Asset, const FName& Key)
{
	if (!Asset) return false;

	if (FMetaData* MD = GetMetaForAsset(Asset))
	{
		MD->RemoveValue(Asset, Key);
		
		if (UPackage* Package = Asset->GetOutermost())
		{
			Package->SetDirtyFlag(true);
		}
		
		return true;
	}
	return false;
}

bool UDeprecatedAssetMetadataLibrary::GetMetaValue(UObject* Asset, const FName& Key, FString& OutValue)
{
	OutValue.Reset();
	
	if (!Asset) return false;

	if (FMetaData* MD = GetMetaForAsset(Asset))
	{
		const FString& Value = MD->GetValue(Asset, Key);
		
		if (!Value.IsEmpty())
		{
			OutValue = Value;
			
			return true;
		}
	}
	
	return false;
}

void UDeprecatedAssetMetadataLibrary::RefreshAssetThumbnail(UObject* Asset)
{
	if (!Asset) return;

	FAssetRegistryModule& AssetRegistryModule =FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	FString Path = Asset->GetPathName();
	
	AssetRegistryModule.AssetRenamed(Asset, Path);

}

bool UDeprecatedAssetMetadataLibrary::GetDeprecatedInfo(UObject* Asset, FDeprecatedAssetInfo& OutInfo)
{
	OutInfo = {};
	
	if (!Asset) return false;

	FString Flag;
	
	if (GetMetaValue(Asset, MetaKey_Deprecated, Flag))
	{
		OutInfo.bIsDeprecated = (Flag.Equals(TEXT("True"), ESearchCase::IgnoreCase) || Flag == TEXT("1"));
	}

	FString ReplacementStr;
	
	if (GetMetaValue(Asset, MetaKey_Replacement, ReplacementStr) && !ReplacementStr.IsEmpty())
	{
		OutInfo.Replacement = TSoftObjectPtr<UObject>(FSoftObjectPath(ReplacementStr));
	}

	

	return true;
}

bool UDeprecatedAssetMetadataLibrary::MarkDeprecated(UObject* Asset, TSoftObjectPtr<UObject> Replacement)
{
	if (!Asset) return false;

	const bool b1 = SetMetaValue(Asset, MetaKey_Deprecated, TEXT("True"));
	
	const bool b2 = Replacement.IsNull()
		? RemoveMetaValue(Asset, MetaKey_Replacement)
		: SetMetaValue(Asset, MetaKey_Replacement, Replacement.ToSoftObjectPath().ToString());

	
	Asset->MarkPackageDirty();

	RefreshAssetThumbnail(Asset);

	return b1 && b2;
}

bool UDeprecatedAssetMetadataLibrary::UnmarkDeprecated(UObject* Asset)
{
	if (!Asset) return false;
	
	const bool b1 = RemoveMetaValue(Asset, MetaKey_Deprecated);
	
	const bool b2 = RemoveMetaValue(Asset, MetaKey_Replacement);

	RefreshAssetThumbnail(Asset);
	
	return b1 || b2;
}

bool UDeprecatedAssetMetadataLibrary::GetReplacement(UObject* Asset, TSoftObjectPtr<UObject>& OutReplacement)
{
	OutReplacement.Reset();
	
	FDeprecatedAssetInfo Info;
	
	if (!GetDeprecatedInfo(Asset, Info)) return false;

	if (Info.bIsDeprecated && (Info.Replacement.IsValid() || !Info.Replacement.IsNull()))
	{
		OutReplacement = Info.Replacement;
		
		return true;
	}
	
	return false;
}



