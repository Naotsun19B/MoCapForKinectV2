// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "MoCapforKinectV2.h"
#include "Modules/ModuleManager.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformFile.h"
#include "KinectAccesser.h"

#include "AllowWindowsPlatformTypes.h"  //
#include <string>	        			// マクロの多重定義を回避
#include "HideWindowsPlatformTypes.h"	//

#define LOCTEXT_NAMESPACE "FMoCapforKinectV2Module"

void FMoCapforKinectV2Module::StartupModule()
{
	//表情認識用のデータベースをコピー
	FString source = GetKinectFaceDir() + TEXT("NuiDatabase");
	FString target = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectDir(), TEXT("Binaries"), TEXT("Win64"), TEXT("NuiDatabase")));
	if (FPlatformFileManager::Get().GetPlatformFile().CopyDirectoryTree(*target, *source, true))
	{
		UE_LOG(MoCapForKinectV2, Log, TEXT("Copied %s to %s"), *source, *target);
	}
	else UE_LOG(MoCapForKinectV2, Log, TEXT("Can't copy database : %s"), *target);

	FKinectAccesser::Create();
}

void FMoCapforKinectV2Module::ShutdownModule()
{
	
	FKinectAccesser::Destroy();
}

FString FMoCapforKinectV2Module::GetKinectFaceDir()
{
	//Kinectのベースパスを取得
	size_t buf;
	if (getenv_s(&buf, nullptr, 0, "KINECTSDK20_DIR"))
	{
		UE_LOG(MoCapForKinectV2, Fatal, TEXT("Read Error"));
	}
	if (buf == 0)
	{
		UE_LOG(MoCapForKinectV2, Fatal, TEXT("Not such environment variable"));
	}

	std::string buffer;
	buffer.resize(buf + 1);
	getenv_s(&buf, &buffer[0], buf, "KINECTSDK20_DIR");
	buffer.resize(std::strlen(buffer.c_str()));

	//../Redist/Face/[x64 or x86]/
	FString path = buffer.c_str();
	FString platform = "";

#ifdef _WIN64
	platform = TEXT("x64");
#elif _WIN32
	platform = TEXT("x86");
#endif

	return FPaths::Combine(path, TEXT("Redist"), TEXT("Face"), platform, TEXT(""));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMoCapforKinectV2Module, MoCapforKinectV2)
