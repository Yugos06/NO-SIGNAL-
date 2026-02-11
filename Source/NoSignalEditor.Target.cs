using UnrealBuildTool;
using System.Collections.Generic;

public class NoSignalEditorTarget : TargetRules
{
    public NoSignalEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;

        ExtraModuleNames.Add("NoSignal");
    }
}
