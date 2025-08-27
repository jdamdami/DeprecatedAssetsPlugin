using UnrealBuildTool;

public class DeprecatedAssetsEditor : ModuleRules
{
	public DeprecatedAssetsEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "Slate", "SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"UnrealEd",
			"AssetRegistry",
			"ContentBrowser",
			"LevelEditor",
			"EditorFramework",
			"DataValidation",
			"MessageLog",
			"Projects",
			"PropertyEditor",
			"InputCore",
			"ApplicationCore",
			"ToolMenus",
			"EditorSubsystem",
			"Kismet",
			"DeprecatedAssets"
		});
	}
}