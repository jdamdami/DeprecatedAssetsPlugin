// Copyright Boneshock Studio AS 2024.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "DeprecatedAssetSettings.generated.h"


UCLASS(Config=Editor, DefaultConfig, meta=(DisplayName="Deprecated Asset Plugin Settings"))
class DEPRECATEDASSETSEDITOR_API UDeprecatedAssetSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	//If it is not enabled, the warnings will only be visible in output log
	UPROPERTY(EditAnywhere, Config, Category="General")
	bool bEnableFullErrorReport = true;

	virtual FName GetCategoryName() const override { return TEXT("Plugins"); }
	virtual FName GetSectionName() const override { return TEXT("Deprecated Asset Plugin"); }
};
