#include "DeprecatedContentBrowserExtender.h"
#include "ContentBrowserModule.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Modules/ModuleManager.h"
#include "Widgets/SWindow.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "PropertyCustomizationHelpers.h"
#include "Framework/Application/SlateApplication.h"
#include "DeprecatedAssetMetadata.h"
#include "Misc/MessageDialog.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#define LOCTEXT_NAMESPACE "DeprecatedAssetsCB"

FDelegateHandle FDeprecatedContentBrowserExtender::Register()
{
	FContentBrowserModule& CBModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	
	auto& Extenders = CBModule.GetAllAssetViewContextMenuExtenders();

	FContentBrowserMenuExtender_SelectedAssets Delegate;
	
	Delegate.BindStatic(&FDeprecatedContentBrowserExtender::OnExtend);
	
	Extenders.Add(Delegate);

	return Delegate.GetHandle();
}

void FDeprecatedContentBrowserExtender::Unregister(FDelegateHandle Handle)
{
	if (!Handle.IsValid()) return;

	FContentBrowserModule& CBModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	
	auto& Extenders = CBModule.GetAllAssetViewContextMenuExtenders();

	Extenders.RemoveAll([&](const FContentBrowserMenuExtender_SelectedAssets& D)
	{
		return D.GetHandle() == Handle;
	});
}

TSharedRef<FExtender> FDeprecatedContentBrowserExtender::OnExtend(const TArray<FAssetData>& SelectedAssets)
{
	TSharedRef<FExtender> Extender = MakeShared<FExtender>();
	
	if (SelectedAssets.Num() == 0) return Extender;

	Extender->AddMenuExtension(
		"GetAssetActions",
		EExtensionHook::After,
		nullptr,
		FMenuExtensionDelegate::CreateStatic(&FDeprecatedContentBrowserExtender::BuildMenu, SelectedAssets)
	);

	return Extender;
}

void FDeprecatedContentBrowserExtender::BuildMenu(FMenuBuilder& MenuBuilder, TArray<FAssetData> SelectedAssets)
{
	bool bAllDeprecated = true;
	
	bool bAllNotDeprecated = true;

	
	for (const FAssetData& AD : SelectedAssets)
	{
		if (UObject* Asset = AD.GetAsset())
		{
			FDeprecatedAssetInfo Info;
			UDeprecatedAssetMetadataLibrary::GetDeprecatedInfo(Asset, Info);

			if (Info.bIsDeprecated)
			{
				bAllNotDeprecated = false;
			}
			else
			{
				bAllDeprecated = false;
			}
		}
	}

	MenuBuilder.BeginSection("DeprecatedAssets", LOCTEXT("DeprecatedAssetsSection", "Deprecated Assets"));
	{
		if (bAllNotDeprecated)
		{
			
			MenuBuilder.AddMenuEntry(
				LOCTEXT("MarkDeprecated", "Mark as Deprecated…"),
				LOCTEXT("MarkDeprecatedTT", "Mark the selected assets as deprecated and optionally set a replacement asset."),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FDeprecatedContentBrowserExtender::Cmd_MarkDeprecated, SelectedAssets))
			);
		}

		
		

		if (bAllDeprecated)
		{
			MenuBuilder.AddMenuEntry(
				LOCTEXT("SetReplacement", "Set Replacement…"),
				LOCTEXT("SetReplacementTT", "Set or change the recommended replacement asset for deprecated assets."),
				FSlateIcon(),FUIAction(FExecuteAction::CreateStatic(&FDeprecatedContentBrowserExtender::Cmd_SetReplacement, SelectedAssets)));
			
			MenuBuilder.AddMenuEntry(
				LOCTEXT("UnmarkDeprecated", "Unmark Deprecated"),
				LOCTEXT("UnmarkDeprecatedTT", "Remove the deprecated mark and replacement info from selected assets."),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FDeprecatedContentBrowserExtender::Cmd_UnmarkDeprecated, SelectedAssets))
			);
		}
	}
	
	MenuBuilder.EndSection();
}

void FDeprecatedContentBrowserExtender::Cmd_MarkDeprecated(TArray<FAssetData> SelectedAssets)
{
	ShowPickReplacementDialog([SelectedAssets](UObject* Picked)
	{
		int32 Count = 0;
		for (const FAssetData& AD : SelectedAssets)
		{
			if (UObject* Asset = AD.GetAsset())
			{
				UDeprecatedAssetMetadataLibrary::MarkDeprecated(Asset, TSoftObjectPtr<UObject>(Picked));
				++Count;
			}
		}
		Notify(FText::Format(LOCTEXT("MarkedNAssets", "Marked {0} asset(s) deprecated."), Count), true);
	});
}

void FDeprecatedContentBrowserExtender::Cmd_SetReplacement(TArray<FAssetData> SelectedAssets)
{
	ShowPickReplacementDialog([SelectedAssets](UObject* Picked)
	{
		int32 Count = 0;
		
		for (const FAssetData& AD : SelectedAssets)
		{
			if (UObject* Asset = AD.GetAsset())
			{
				FDeprecatedAssetInfo Info;
				
				UDeprecatedAssetMetadataLibrary::GetDeprecatedInfo(Asset, Info);
				
				if (!Info.bIsDeprecated)
				{
					Info.bIsDeprecated = true;
				}
				
				UDeprecatedAssetMetadataLibrary::MarkDeprecated(Asset, TSoftObjectPtr<UObject>(Picked));
				
				++Count;
			}
		}
		
		Notify(FText::Format(LOCTEXT("SetReplacementNAssets", "Set replacement for {0} asset(s)."), Count), true);
	});
}

void FDeprecatedContentBrowserExtender::Cmd_UnmarkDeprecated(TArray<FAssetData> SelectedAssets)
{
	int32 Count = 0;
	
	for (const FAssetData& AD : SelectedAssets)
	{
		if (UObject* Asset = AD.GetAsset())
		{
			UDeprecatedAssetMetadataLibrary::UnmarkDeprecated(Asset);
			
			++Count;
		}
	}
	
	Notify(FText::Format(LOCTEXT("UnmarkedNAssets", "Unmarked {0} asset(s)."), Count), true);
}

void FDeprecatedContentBrowserExtender::ShowPickReplacementDialog(TFunction<void(UObject*)> OnPicked,UObject* InitialReplacement /*= nullptr*/)
{
    TSharedRef<SWindow> Window = SNew(SWindow)
        .Title(LOCTEXT("PickReplacementTitle", "Pick Replacement Asset (optional)"))
        .ClientSize(FVector2D(500, 120))
        .SupportsMinimize(false)
        .SupportsMaximize(false);

    
    TSharedPtr<TWeakObjectPtr<UObject>> PickedPtr = MakeShared<TWeakObjectPtr<UObject>>(InitialReplacement);
	
    TSharedPtr<FString> PathPtr = MakeShared<FString>(InitialReplacement ? InitialReplacement->GetPathName() : FString());

    Window->SetContent(
        SNew(SVerticalBox)

        + SVerticalBox::Slot()
        .Padding(10)
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("PickReplacementHelp", "Choose a recommended replacement asset (you can leave it empty)."))
        ]

        + SVerticalBox::Slot()
        .Padding(10)
        .AutoHeight()
        [
            SNew(SObjectPropertyEntryBox)
            .AllowedClass(UObject::StaticClass())
            .AllowClear(true)
            .ObjectPath_Lambda([PathPtr]() -> FString
            {
                return *PathPtr; // Always pull current path
            })
            .OnObjectChanged_Lambda([PickedPtr, PathPtr](const FAssetData& InAssetData)
            {
                UObject* Asset = InAssetData.GetAsset();
                *PickedPtr = Asset;
                *PathPtr = Asset ? Asset->GetPathName() : FString();
            })
        ]

        + SVerticalBox::Slot()
        .Padding(10)
        .AutoHeight()
        .HAlign(HAlign_Right)
        [
            SNew(SButton)
            .Text(LOCTEXT("Confirm", "OK"))
            .OnClicked_Lambda([Window, OnPicked, PickedPtr]()
            {
                OnPicked(PickedPtr->Get());
                Window->RequestDestroyWindow();
                return FReply::Handled();
            })
        ]
    );

    FSlateApplication::Get().AddWindow(Window);
}

void FDeprecatedContentBrowserExtender::Notify(const FText& Message, bool bSuccess)
{
	FNotificationInfo Info(Message);
	
	Info.bFireAndForget = true;
	
	Info.FadeOutDuration = 0.5f;
	
	Info.ExpireDuration = 2.0f;
	
	if (TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info))
	{
		Notification->SetCompletionState(bSuccess ? SNotificationItem::CS_Success : SNotificationItem::CS_Fail);
	}
}

#undef LOCTEXT_NAMESPACE
