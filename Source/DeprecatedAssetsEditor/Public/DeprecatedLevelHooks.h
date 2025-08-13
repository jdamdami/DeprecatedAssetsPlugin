#pragma once

#include "CoreMinimal.h"

class FObjectPreSaveContext;

class FDeprecatedLevelHooks
{
public:
	static void Register(FDelegateHandle& OutOnActorAddedHandle,FDelegateHandle& OutPreSaveWorldHandle,FDelegateHandle& OutOnPropertyChangedHandle,FDelegateHandle& OutOnObjectPreSaveHandle);

	static void Unregister(FDelegateHandle& OnActorAddedHandle,FDelegateHandle& PreSaveWorldHandle,FDelegateHandle& OnPropertyChangedHandle,FDelegateHandle& OnObjectPreSaveHandle);

private:
	
	static bool bWarningShownThisSave;
	
	static TSet<FName> PackagesCheckedThisSave;

	static void OnActorAdded(AActor* Actor);
	
	static void DeferredCheckActor(TWeakObjectPtr<AActor> WeakActor);

	static void OnPropertyChanged(UObject* Object, struct FPropertyChangedEvent& Event);

	static void OnPreSaveWorld(UWorld* World, FObjectPreSaveContext SaveContext);
	
	static void OnObjectPreSave(UObject* Object, FObjectPreSaveContext SaveContext);

	static void ReportDeprecatedAsset(UObject* DeprecatedAsset);
	
	static bool IsAssetDeprecated(UObject* Asset, TSoftObjectPtr<UObject>& OutReplacement);

	static void ScanWorldForDeprecatedAssets(UWorld* World);
	
	static void ScanPackageForDeprecatedAssets(UPackage* Package);
	
	static void SchedulePackagesCheckedReset();
};
