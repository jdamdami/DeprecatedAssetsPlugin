#pragma once

#include "Modules/ModuleManager.h"

class FDeprecatedAssetsEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	
	virtual void ShutdownModule() override;

private:

	FDelegateHandle CBExtenderHandle;

	FDelegateHandle OnActorAddedHandle;
	
	FDelegateHandle PreSaveWorldHandle;
	
	FDelegateHandle OnPropertyChangedHandle;
	
	FDelegateHandle OnObjectPreSaveHandle;

	
	
	void RegisterContentBrowserExtenders();
	
	void UnregisterContentBrowserExtenders();

	void RegisterLevelHooks();
	
	void UnregisterLevelHooks();


	
};
