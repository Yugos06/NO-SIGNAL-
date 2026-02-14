using UnrealBuildTool;
using System.Collections.Generic;

public class NoSignalEditorTarget : TargetRules
{
    public NoSignalEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        ExtraModuleNames.Add("NoSignal");
    }
}
