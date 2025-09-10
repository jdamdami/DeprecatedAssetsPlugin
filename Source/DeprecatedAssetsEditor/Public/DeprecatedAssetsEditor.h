#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDeprecatedAssetPlugin, Log, All);

class FDeprecatedAssetsEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	
	virtual void ShutdownModule() override;

	static TSharedRef<SWidget> OnDeprecatedAssetIconGenerate(const FAssetData& AssetData);

private:

	FDelegateHandle CBExtenderHandle;

	FDelegateHandle OnActorAddedHandle;
	
	FDelegateHandle PreSaveWorldHandle;
	
	FDelegateHandle OnPropertyChangedHandle;
	
	FDelegateHandle OnObjectPreSaveHandle;

	static TUniquePtr<FSlateStyleSet> DeprecatedStyleSet;
	
	static FName DeprecatedAssetIconName;
	
	void RegisterContentBrowserExtenders();
	
	void UnregisterContentBrowserExtenders();

	void RegisterLevelHooks();
	
	void UnregisterLevelHooks();

	void RegisterSlateStyle();
	
	void UnregisterSlateStyle();

	void RegisterConsoleCommands();

	void UnregisterConsoleCommands();

	void HandleToggleErrorReportConsoleCommand(const TArray<FString>& Args);


	
};
