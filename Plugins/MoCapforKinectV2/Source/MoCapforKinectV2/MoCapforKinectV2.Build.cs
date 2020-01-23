// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System;
using System.IO;

public class MoCapforKinectV2 : ModuleRules
{
	public MoCapforKinectV2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "AnimGraphRuntime",
            }
            );

        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",	
			}
			);

        ///////////////////////////////////////////////////////
        // Kinectのライブラリを追加
        // Kinectの表情認識に必要なdllとデータベースをコピー

        string kinectPath = Environment.GetEnvironmentVariable("KINECTSDK20_DIR");
        string libPath = Path.Combine(kinectPath, "Lib");
        string sourceDirectory = Path.Combine(kinectPath, "Redist", "Face");
        string pluginBinary = Path.Combine(ModuleDirectory, "..", "..", "Binaries");

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            libPath = Path.Combine(libPath, "x64");
            sourceDirectory = Path.Combine(sourceDirectory, "x64");
            pluginBinary = Path.Combine(pluginBinary, "Win64"); 
        }
        else if(Target.Platform == UnrealTargetPlatform.Win32)
        {
            libPath = Path.Combine(libPath, "x86");
            sourceDirectory = Path.Combine(sourceDirectory, "x86");
            pluginBinary = Path.Combine(pluginBinary, "Win32");
        }
        else throw new Exception(String.Format("Unsupported platform {0}", Target.Platform.ToString()));

        PublicIncludePaths.Add(Path.Combine(kinectPath, "inc") + Path.DirectorySeparatorChar);

        PublicAdditionalLibraries.AddRange(
            new string[] 
            {
                Path.Combine(libPath, "Kinect20.Face.lib"),
                Path.Combine(libPath, "Kinect20.lib"),
            }
            );

        CopyDirectory(Path.Combine(sourceDirectory, "NuiDatabase"), Path.Combine(pluginBinary, "NuiDatabase"));
    }

    private void CopyFile(string Source, string Destination)
    {
        Console.WriteLine("Coppying {0} to {1}", Source, Destination);

        if (File.Exists(Destination))
            File.SetAttributes(Destination, File.GetAttributes(Source) & ~FileAttributes.ReadOnly);

        try { File.Copy(Source, Destination, true); }
        catch(Exception e) { Console.WriteLine("Failed to copy file : {0}", e.Message); }
    }

    private void CopyDirectory(string Source, string Destination)
    {
        Console.WriteLine("Coppying {0} to {1}", Source, Destination);

        DirectoryInfo sourceDir = new DirectoryInfo(Source);
        DirectoryInfo destinationDir = new DirectoryInfo(Destination);

        if (!destinationDir.Exists)
        {
            destinationDir.Create();
            destinationDir.Attributes = sourceDir.Attributes;
        }

        try
        {
            foreach (FileInfo fileInfo in sourceDir.GetFiles())
                fileInfo.CopyTo(destinationDir.FullName + Path.DirectorySeparatorChar + fileInfo.Name, true);
            foreach (DirectoryInfo directoryInfo in sourceDir.GetDirectories())
                CopyDirectory(directoryInfo.FullName, destinationDir.FullName + Path.DirectorySeparatorChar + directoryInfo.Name);
        }
        catch(Exception e) { Console.WriteLine("Failed to copy file : {0}", e.Message); }
    }
}
