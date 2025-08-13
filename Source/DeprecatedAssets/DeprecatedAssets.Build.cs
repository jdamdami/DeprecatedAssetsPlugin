using UnrealBuildTool;

public class DeprecatedAssets : ModuleRules
{
	public DeprecatedAssets(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"Projects"
		});
	}
}