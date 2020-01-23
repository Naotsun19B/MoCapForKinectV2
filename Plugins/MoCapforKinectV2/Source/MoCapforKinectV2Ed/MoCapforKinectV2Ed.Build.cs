// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MoCapforKinectV2Ed : ModuleRules
{
    public MoCapforKinectV2Ed(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "MoCapForKinectV2",
            }
            );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "AnimGraph",
                "BlueprintGraph",
                "Persona",
                "UnrealEd",
                "AnimGraphRuntime",
                "SlateCore",
            }
            );
    }
}