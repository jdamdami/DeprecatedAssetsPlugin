// Copyright Boneshock Studio AS 2024.

#pragma once

#include "CoreMinimal.h"
#include "ContentBrowserFrontEndFilterExtension.h"
#include "DeprecatedAssetsFilterExtension.generated.h"

/**
 * Content Browser filter extension for deprecated assets
 */
UCLASS()
class DEPRECATEDASSETSEDITOR_API UDeprecatedAssetsFilterExtension : public UContentBrowserFrontEndFilterExtension
{
	GENERATED_BODY()

public:
	
	virtual void AddFrontEndFilterExtensions(TSharedPtr<class FFrontendFilterCategory> DefaultCategory,TArray<TSharedRef<class FFrontendFilter>>& InOutFilterList) const override;
};
