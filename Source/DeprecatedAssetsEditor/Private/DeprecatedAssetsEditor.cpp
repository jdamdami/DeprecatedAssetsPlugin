#include "DeprecatedAssetsEditor.h"
#include "DeprecatedContentBrowserExtender.h"
#include "DeprecatedLevelHooks.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FDeprecatedAssetsEditorModule, DeprecatedAssetsEditor)

void FDeprecatedAssetsEditorModule::StartupModule()
{
	RegisterContentBrowserExtenders();
	
	RegisterLevelHooks();
}

void FDeprecatedAssetsEditorModule::ShutdownModule()
{
	UnregisterLevelHooks();
	
	UnregisterContentBrowserExtenders();
}

void FDeprecatedAssetsEditorModule::RegisterContentBrowserExtenders()
{
	CBExtenderHandle = FDeprecatedContentBrowserExtender::Register();
}

void FDeprecatedAssetsEditorModule::UnregisterContentBrowserExtenders()
{
	if (CBExtenderHandle.IsValid())
	{
		FDeprecatedContentBrowserExtender::Unregister(CBExtenderHandle);
		
		CBExtenderHandle.Reset();
	}
}

void FDeprecatedAssetsEditorModule::RegisterLevelHooks()
{
	
	FDeprecatedLevelHooks::Register(
		OnActorAddedHandle,
		PreSaveWorldHandle,
		OnPropertyChangedHandle,
		OnObjectPreSaveHandle   
	);
}

void FDeprecatedAssetsEditorModule::UnregisterLevelHooks()
{
	FDeprecatedLevelHooks::Unregister(
		OnActorAddedHandle,
		PreSaveWorldHandle,
		OnPropertyChangedHandle,
		OnObjectPreSaveHandle    
	);

	
	OnActorAddedHandle.Reset();
	PreSaveWorldHandle.Reset();
	OnPropertyChangedHandle.Reset();
	OnObjectPreSaveHandle.Reset();
}
