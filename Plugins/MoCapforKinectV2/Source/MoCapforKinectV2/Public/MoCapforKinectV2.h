// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "AllowWindowsPlatformTypes.h"  //
#include "Kinect.h"		        	    // マクロの多重定義を回避
#include "Kinect.Face.h"				//
#include "HideWindowsPlatformTypes.h"	//

class FMoCapforKinectV2Module : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	// Get the path of the directory containing the dll and database required for face tracking
	static FString GetKinectFaceDir();
};

DEFINE_LOG_CATEGORY_STATIC(MoCapForKinectV2, Log, All);

#define GETENUMSTRING(etype, evalue) ( (FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true) != nullptr) ? FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true)->GetNameStringByIndex((int32)evalue) : FString("Invalid - are you sure enum uses UENUM() macro?") )

UENUM(BlueprintType)
enum class ETrackingState : uint8
{
	TS_NotTracked	UMETA(DisplayName = "NotTracked"),
	TS_Inferred		UMETA(DisplayName = "Inferred"),
	TS_Tracked		UMETA(DisplayName = "Tracked"),
	TS_Unknown		UMETA(DisplayName = "Unknown"),
};

UENUM(BlueprintType)
enum class EJointType : uint8
{
	JT_SpineBase		UMETA(DisplayName = "SpineBase"),
	JT_SpineMid			UMETA(DisplayName = "SpineMid"),
	JT_Neck				UMETA(DisplayName = "Neck"),
	JT_Head				UMETA(DisplayName = "Head"),
	JT_ShoulderLeft		UMETA(DisplayName = "ShoulderLeft"),
	JT_ElbowLeft		UMETA(DisplayName = "ElbowLeft"),
	JT_WristLeft		UMETA(DisplayName = "WristLeft"),
	JT_HandLeft			UMETA(DisplayName = "HandLeft"),
	JT_ShoulderRight	UMETA(DisplayName = "ShoulderRight"),
	JT_ElbowRight		UMETA(DisplayName = "ElbowRight"),
	JT_WristRight		UMETA(DisplayName = "WristRight"),
	JT_HandRight		UMETA(DisplayName = "HandRight"),
	JT_HipLeft			UMETA(DisplayName = "HipLeft"),
	JT_KneeLeft			UMETA(DisplayName = "KneeLeft"),
	JT_AnkleLeft		UMETA(DisplayName = "AnkleLeft"),
	JT_FootLeft			UMETA(DisplayName = "FootLeft"),
	JT_HipRight			UMETA(DisplayName = "HipRight"),
	JT_KneeRight		UMETA(DisplayName = "KneeRight"),
	JT_AnkleRight		UMETA(DisplayName = "AnkleRight"),
	JT_FootRight		UMETA(DisplayName = "FootRight"),
	JT_SpineShoulder	UMETA(DisplayName = "SpineShoulder"),
	JT_HandTipLeft		UMETA(DisplayName = "HandTipLeft"),
	JT_ThumbLeft		UMETA(DisplayName = "ThumbLeft"),
	JT_HandTipRight		UMETA(DisplayName = "HandTipRight"),
	JT_ThumbRight		UMETA(DisplayName = "ThumbRight"),
	JT_Unknown			UMETA(DisplayName = "Unknown"),
};

UENUM(BlueprintType)
enum class EHandState : uint8
{
	HS_NotTracked	UMETA(DisplayName = "NotTracked"),
	HS_Open			UMETA(DisplayName = "Open"),
	HS_Closed		UMETA(DisplayName = "Closed"),
	HS_Lasso		UMETA(DisplayName = "Lasso"),
	HS_Unknown		UMETA(DisplayName = "Unknown"),
};

UENUM(BlueprintType)
enum class EFaceResult : uint8
{
	FR_Unknown	UMETA(DisplayName = "Unknown"),
	FR_No		UMETA(DisplayName = "No"),
	FR_Maybe	UMETA(DisplayName = "Maybe"),
	FR_Yes		UMETA(DisplayName = "Yes"),
};
