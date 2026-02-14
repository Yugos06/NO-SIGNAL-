using UnrealBuildTool;
using System.Collections.Generic;

public class NoSignalTarget : TargetRules
{
    public NoSignalTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        ExtraModuleNames.Add("NoSignal");
    }
}
