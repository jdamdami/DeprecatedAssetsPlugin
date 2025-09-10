#include "DeprecatedAssetsEditor.h"

#include "ContentBrowserModule.h"
#include "DeprecatedAssetFrontendFilter.h"
#include "DeprecatedAssetMetadata.h"
#include "DeprecatedAssetSettings.h"
#include "DeprecatedContentBrowserExtender.h"
#include "DeprecatedLevelHooks.h"
#include "FrontendFilterBase.h"
#include "Interfaces/IPluginManager.h"
#include "Modules/ModuleManager.h"
#include "Styling/SlateStyleRegistry.h"

DEFINE_LOG_CATEGORY(LogDeprecatedAssetPlugin);

TUniquePtr<FSlateStyleSet> FDeprecatedAssetsEditorModule::DeprecatedStyleSet;

FName FDeprecatedAssetsEditorModule::DeprecatedAssetIconName(TEXT("DeprecatedAssets.Icon"));

IMPLEMENT_MODULE(FDeprecatedAssetsEditorModule, DeprecatedAssetsEditor)

void FDeprecatedAssetsEditorModule::StartupModule()
{
	RegisterContentBrowserExtenders();
	
	RegisterLevelHooks();

	RegisterSlateStyle();

	RegisterConsoleCommands();
}

void FDeprecatedAssetsEditorModule::ShutdownModule()
{
	UnregisterLevelHooks();
	
	UnregisterContentBrowserExtenders();

	UnregisterSlateStyle();

	UnregisterConsoleCommands();
}

TSharedRef<SWidget> FDeprecatedAssetsEditorModule::OnDeprecatedAssetIconGenerate(const FAssetData& AssetData)
{
	UObject* AssetObj = AssetData.GetAsset();
	
	if (!AssetObj)
	{
		return SNew(SBox); 
	}

	FDeprecatedAssetInfo Info;
	
	if (UDeprecatedAssetMetadataLibrary::GetDeprecatedInfo(AssetObj, Info) && Info.bIsDeprecated)
	{
		if (DeprecatedStyleSet.IsValid())
		{
			const FSlateBrush* Brush = DeprecatedStyleSet->GetBrush(DeprecatedAssetIconName);

			return SNew(SBox)
				.Padding(4.0f, 4.0f, 0.0f, 0.0f)
				[
					SNew(SImage)
					.Image(Brush)
					.ToolTipText(NSLOCTEXT("DeprecatedAssets", "Tooltip_Deprecated", "This asset is deprecated."))
				];
		}
		else
		{
			return SNew(STextBlock).Text(FText::FromString("DEPRECATED"));
		}
	}

	return SNew(SBox);

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

void FDeprecatedAssetsEditorModule::RegisterSlateStyle()
{
	DeprecatedStyleSet = MakeUnique<FSlateStyleSet>(TEXT("DeprecatedAssetsStyle"));
	
	DeprecatedStyleSet->SetContentRoot(IPluginManager::Get().FindPlugin(TEXT("DeprecatedAssets"))->GetBaseDir() / TEXT("Resources"));

	
	const FString IconPath = DeprecatedStyleSet->RootToContentDir(TEXT("WarningIcon.svg"));
	
	DeprecatedStyleSet->Set(DeprecatedAssetIconName, new FSlateVectorImageBrush(IconPath, CoreStyleConstants::Icon20x20));

	FSlateStyleRegistry::RegisterSlateStyle(*DeprecatedStyleSet);
	
	FSlateApplication::Get().GetRenderer()->ReloadTextureResources();

	FContentBrowserModule& CBModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	FAssetViewExtraStateGenerator StateGenerator(
		FOnGenerateAssetViewExtraStateIndicators::CreateStatic(&FDeprecatedAssetsEditorModule::OnDeprecatedAssetIconGenerate),
		FOnGenerateAssetViewExtraStateIndicators()
	);

	CBModule.AddAssetViewExtraStateGenerator(StateGenerator);

}

void FDeprecatedAssetsEditorModule::UnregisterSlateStyle()
{
	if (DeprecatedStyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*DeprecatedStyleSet);
		
		DeprecatedStyleSet.Reset();
	}
}

void FDeprecatedAssetsEditorModule::RegisterConsoleCommands()
{
	IConsoleManager::Get().RegisterConsoleCommand(
			TEXT("DeprecatedAssets.ErrorReport"),
			TEXT("Toggle full error reporting for deprecated assets"),
			FConsoleCommandWithArgsDelegate::CreateRaw(this, &FDeprecatedAssetsEditorModule::HandleToggleErrorReportConsoleCommand),
			ECVF_Default
		);
}

void FDeprecatedAssetsEditorModule::UnregisterConsoleCommands()
{
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("DeprecatedAssets.ErrorReport"), false);
}

void FDeprecatedAssetsEditorModule::HandleToggleErrorReportConsoleCommand(const TArray<FString>& Args)
{
	UDeprecatedAssetSettings* Settings = GetMutableDefault<UDeprecatedAssetSettings>();
	
	if (!Settings)
	{
		return;
	}

	if (Args.Num() == 0)
	{
		UE_LOG(LogDeprecatedAssetPlugin, Error, TEXT("Missing argument. Usage: DeprecatedAssets.ErrorReport <0|1>"));
		
		return;
	}

	const FString& Arg = Args[0];

	if (Arg == TEXT("1"))
	{
		Settings->bEnableFullErrorReport = true;
	}
	else if (Arg == TEXT("0"))
	{
		Settings->bEnableFullErrorReport = false;
	}
	else
	{
		UE_LOG(LogDeprecatedAssetPlugin, Error, TEXT("Invalid argument '%s'. Usage: DeprecatedAssets.ErrorReport <0|1>"), *Arg);
		
		return;
	}

	UE_LOG(LogDeprecatedAssetPlugin, Display, TEXT("Deprecated Asset Plugin: bEnableFullErrorReport = %s"),Settings->bEnableFullErrorReport ? TEXT("true") : TEXT("false"));
}
