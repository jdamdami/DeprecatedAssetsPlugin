#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/SoftObjectPtr.h"
#include "UObject/MetaData.h" 
#include "DeprecatedAssetMetadata.generated.h"

USTRUCT(BlueprintType)
struct FDeprecatedAssetInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DeprecatedAssets")
	
	bool bIsDeprecated = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DeprecatedAssets")
	
	TSoftObjectPtr<UObject> Replacement;
};


UCLASS()
class DEPRECATEDASSETSEDITOR_API UDeprecatedAssetMetadataLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="DeprecatedAssets", meta=(DisplayName="Get Deprecated Asset Info"))
	static bool GetDeprecatedInfo(UObject* Asset, FDeprecatedAssetInfo& OutInfo);

	UFUNCTION(BlueprintCallable, Category="DeprecatedAssets", meta=(DisplayName="Mark Asset Deprecated"))
	static bool MarkDeprecated(UObject* Asset, TSoftObjectPtr<UObject> Replacement);

	UFUNCTION(BlueprintCallable, Category="DeprecatedAssets", meta=(DisplayName="Unmark Asset Deprecated"))
	static bool UnmarkDeprecated(UObject* Asset);

	UFUNCTION(BlueprintCallable, Category="DeprecatedAssets", meta=(DisplayName="Get Replacement For Deprecated Asset"))
	static bool GetReplacement(UObject* Asset, TSoftObjectPtr<UObject>& OutReplacement);

	static const FName MetaKey_Deprecated;

	static const FName MetaKey_Replacement;

private:

	static FMetaData* GetMetaForAsset(UObject* Asset);

	static FString MakeCompositeKey(UObject* Asset, const FName& Key);
	
	static bool SetMetaValue(UObject* Asset, const FName& Key, const FString& Value);
	
	static bool RemoveMetaValue(UObject* Asset, const FName& Key);
	
	static bool GetMetaValue(UObject* Asset, const FName& Key, FString& OutValue);


	
};
