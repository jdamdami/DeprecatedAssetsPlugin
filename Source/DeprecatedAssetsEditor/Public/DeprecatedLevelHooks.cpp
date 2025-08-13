#include "DeprecatedLevelHooks.h"

#include "Editor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "DeprecatedAssetMetadata.h"
#include "Logging/MessageLog.h"
#include "Misc/MessageDialog.h"
#include "UObject/Package.h"
#include "UObject/ObjectSaveContext.h"
#include "UObject/SoftObjectPtr.h"
#include "TimerManager.h"

#define LOCTEXT_NAMESPACE "DeprecatedLevelHooks"

bool FDeprecatedLevelHooks::bWarningShownThisSave = false;

TSet<FName> FDeprecatedLevelHooks::PackagesCheckedThisSave;

bool FDeprecatedLevelHooks::IsAssetDeprecated(UObject* Asset, TSoftObjectPtr<UObject>& OutReplacement)
{
	if (!Asset) return false;

	FDeprecatedAssetInfo Info;
	
	UDeprecatedAssetMetadataLibrary::GetDeprecatedInfo(Asset, Info);
	
	OutReplacement = Info.Replacement;
	
	return Info.bIsDeprecated;
}

void FDeprecatedLevelHooks::ReportDeprecatedAsset(UObject* DeprecatedAsset)
{
	if (!DeprecatedAsset) return;

	TSoftObjectPtr<UObject> Replacement;
	
	IsAssetDeprecated(DeprecatedAsset, Replacement);

	FMessageLog Log("DataValidation");
	
	Log.Error(FText::Format(
		LOCTEXT("DeprecatedAssetUsed", "Asset '{0}' is deprecated. Use '{1}' instead."),
		FText::FromString(DeprecatedAsset->GetPathName()),
		FText::FromString(Replacement.IsNull() ? TEXT("<none specified>") : Replacement.ToSoftObjectPath().ToString())
	));
	
	Log.Open();
}

void FDeprecatedLevelHooks::Register(FDelegateHandle& OutOnActorAddedHandle,FDelegateHandle& OutPreSaveWorldHandle,FDelegateHandle& OutOnPropertyChangedHandle,FDelegateHandle& OutOnObjectPreSaveHandle)
{
	if (GEditor)
	{
		OutOnActorAddedHandle = GEditor->OnLevelActorAdded().AddStatic(&FDeprecatedLevelHooks::OnActorAdded);
	}

	OutPreSaveWorldHandle = FEditorDelegates::PreSaveWorldWithContext.AddStatic(&FDeprecatedLevelHooks::OnPreSaveWorld);

	OutOnPropertyChangedHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.AddStatic(&FDeprecatedLevelHooks::OnPropertyChanged);

	OutOnObjectPreSaveHandle = FCoreUObjectDelegates::OnObjectPreSave.AddStatic(&FDeprecatedLevelHooks::OnObjectPreSave);
}

void FDeprecatedLevelHooks::Unregister(FDelegateHandle& OnActorAddedHandle,FDelegateHandle& PreSaveWorldHandle,FDelegateHandle& OnPropertyChangedHandle,FDelegateHandle& OnObjectPreSaveHandle)
{
	if (GEditor && OnActorAddedHandle.IsValid())
	{
		GEditor->OnLevelActorAdded().Remove(OnActorAddedHandle);
	}
	
	if (PreSaveWorldHandle.IsValid())
	{
		FEditorDelegates::PreSaveWorldWithContext.Remove(PreSaveWorldHandle);
	}
	
	if (OnPropertyChangedHandle.IsValid())
	{
		FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(OnPropertyChangedHandle);
	}
	
	if (OnObjectPreSaveHandle.IsValid())
	{
		FCoreUObjectDelegates::OnObjectPreSave.Remove(OnObjectPreSaveHandle);
	}
}

void FDeprecatedLevelHooks::OnActorAdded(AActor* Actor)
{
	if (!Actor) return;

	if (UWorld* World = Actor->GetWorld())
	{
		FTimerDelegate TimerDel;
		
		TimerDel.BindStatic(&FDeprecatedLevelHooks::DeferredCheckActor, TWeakObjectPtr<AActor>(Actor));
		
		World->GetTimerManager().SetTimerForNextTick(TimerDel);
	}
}

void FDeprecatedLevelHooks::DeferredCheckActor(TWeakObjectPtr<AActor> WeakActor)
{
	if (!WeakActor.IsValid()) return;

	AActor* Actor = WeakActor.Get();

	auto CheckObject = [](UObject* Obj)
	{
		TSoftObjectPtr<UObject> Replacement;
		
		if (IsAssetDeprecated(Obj, Replacement))
		{
			ReportDeprecatedAsset(Obj);
		}
	};

	if (UStaticMeshComponent* SMC = Actor->FindComponentByClass<UStaticMeshComponent>())
	{
		CheckObject(SMC->GetStaticMesh());
		
		for (int32 Index = 0; Index < SMC->GetNumMaterials(); ++Index)
		{
			CheckObject(SMC->GetMaterial(Index));
		}
			
	}

	if (USkeletalMeshComponent* SKC = Actor->FindComponentByClass<USkeletalMeshComponent>())
	{
		CheckObject(SKC->SkeletalMesh);
		
		for (int32 Index = 0; Index < SKC->GetNumMaterials(); ++Index)
		{
			CheckObject(SKC->GetMaterial(Index));
		}
			
	}
}

void FDeprecatedLevelHooks::OnPropertyChanged(UObject* Object, FPropertyChangedEvent& Event)
{
	if (!Object || !Event.Property) return;

	const FName PropertyName = Event.Property->GetFName();

	if (UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(Object))
	{
		if (PropertyName == TEXT("StaticMesh"))
		{
			if (UStaticMesh* Mesh = SMC->GetStaticMesh())
			{
				TSoftObjectPtr<UObject> Replacement;
				
				if (IsAssetDeprecated(Mesh, Replacement))
				{
					ReportDeprecatedAsset(Mesh);
				}
					
			}
		}
	}
	if (USkeletalMeshComponent* SKC = Cast<USkeletalMeshComponent>(Object))
	{
		if (PropertyName == TEXT("SkeletalMesh"))
		{
			if (USkeletalMesh* Mesh = SKC->SkeletalMesh)
			{
				TSoftObjectPtr<UObject> Replacement;
				
				if (IsAssetDeprecated(Mesh, Replacement))
				{
					ReportDeprecatedAsset(Mesh);
				}
					
			}
		}
	}
}

void FDeprecatedLevelHooks::OnPreSaveWorld(UWorld* World, FObjectPreSaveContext)
{
	UE_LOG(LogTemp, Error, TEXT("PRE SAVE WORLD CALLED"));
	
	bWarningShownThisSave = false;
	
	PackagesCheckedThisSave.Reset();
	
	ScanWorldForDeprecatedAssets(World);
}

void FDeprecatedLevelHooks::OnObjectPreSave(UObject* Object, FObjectPreSaveContext)
{
	if (!Object) return;

	UE_LOG(LogTemp, Error, TEXT("OBJECT PRESAVE CALLED"));

	if (UPackage* Package = Object->GetOutermost())
	{
		if (!PackagesCheckedThisSave.Contains(Package->GetFName()))
		{
			UE_LOG(LogTemp, Error, TEXT("OBJECT PRESAVE PASSED"));
			
			PackagesCheckedThisSave.Add(Package->GetFName());
			
			SchedulePackagesCheckedReset();
			
			ScanPackageForDeprecatedAssets(Package);
		}
	}
}

void FDeprecatedLevelHooks::ScanWorldForDeprecatedAssets(UWorld* World)
{
	if (!World || bWarningShownThisSave) return;

	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

	const FAssetIdentifier WorldId(World->GetOutermost()->GetFName());
	
	using namespace UE::AssetRegistry;

	TArray<FAssetIdentifier> DependencyIds;
	
	if (!AssetRegistry.GetDependencies(WorldId, DependencyIds, EDependencyCategory::Package, FDependencyQuery()))
	{
		return;
	}
		
	for (const FAssetIdentifier& DepId : DependencyIds)
	{
		if (DepId.PackageName.IsNone())
		{
			continue;
		}

		TArray<FAssetData> AssetsInDependencyPackage;
		
		AssetRegistry.GetAssetsByPackageName(DepId.PackageName, AssetsInDependencyPackage);

		for (const FAssetData& AssetData : AssetsInDependencyPackage)
		{
			if (UObject* Asset = AssetData.GetAsset())
			{
				TSoftObjectPtr<UObject> Replacement;
				
				if (IsAssetDeprecated(Asset, Replacement))
				{
					ReportDeprecatedAsset(Asset);
					
					FMessageDialog::Open(EAppMsgType::Ok,
						LOCTEXT("DeprecatedFoundOnWorldSave", "This map references deprecated assets. See the Data Validation log for details."));
					
					bWarningShownThisSave = true;
					
					return;
				}
			}
		}
	}
}

void FDeprecatedLevelHooks::ScanPackageForDeprecatedAssets(UPackage* Package)
{
	UE_LOG(LogTemp, Error, TEXT("ScanPackageForDeprecatedAssets"));

	if (!Package || bWarningShownThisSave) return;

	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

	const FAssetIdentifier PackageId(Package->GetFName());
	
	using namespace UE::AssetRegistry;

	TArray<FAssetIdentifier> DependencyIds;
	
	if (!AssetRegistry.GetDependencies(PackageId, DependencyIds, EDependencyCategory::Package, FDependencyQuery()))
	{
		return;
	}
		
	for (const FAssetIdentifier& DepId : DependencyIds)
	{
		if (DepId.PackageName.IsNone())
		{
			continue;
		}
			
		TArray<FAssetData> AssetsInDependencyPackage;
		
		AssetRegistry.GetAssetsByPackageName(DepId.PackageName, AssetsInDependencyPackage);

		for (const FAssetData& AssetData : AssetsInDependencyPackage)
		{
			if (UObject* Asset = AssetData.GetAsset())
			{
				TSoftObjectPtr<UObject> Replacement;
				
				if (IsAssetDeprecated(Asset, Replacement))
				{
					ReportDeprecatedAsset(Asset);
					
					FMessageDialog::Open(EAppMsgType::Ok,
						LOCTEXT("DeprecatedFoundOnPackageSave", "This asset saves references to deprecated assets. See the Data Validation log for details."));

					bWarningShownThisSave = true;

					return;
				}
			}
		}
	}
}

void FDeprecatedLevelHooks::SchedulePackagesCheckedReset()
{
	static bool bResetScheduled = false;
	
	if (bResetScheduled) return;

	bResetScheduled = true;

	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([](float)
	{
		PackagesCheckedThisSave.Empty();
		UE_LOG(LogTemp, Error, TEXT("PackagesCheckedThisSave RESET"));
		bResetScheduled = false;
		bWarningShownThisSave = false;
		return false; 
	}), 0.5f);
}

#undef LOCTEXT_NAMESPACE
