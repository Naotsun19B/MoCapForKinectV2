// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KinectComponent.h"
#include "KinectBPLibrary.generated.h"

UCLASS()
class MOCAPFORKINECTV2_API UKinectBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// Get vaild body index of found first
	UFUNCTION(BlueprintCallable, Category = "MoCapForKinectV2|BodyIndex")
		static int GetVaildBodyIndex();
	
	// Get Kinect initializeation status
	UFUNCTION(BlueprintCallable, Category = "MoCapForKinectV2")
		static bool GetIsKinectSensorInitialized();

	// Get Kinect state
	UFUNCTION(BlueprintCallable, Category = "MoCapForKinectV2")
		static bool GetIsKinectSensorEnable();

	// Initialize Kinect sensor
	UFUNCTION(BlueprintCallable, Category = "MoCapForKinectV2")
		static bool InitializeKinectSensor();

	// Close Kinect sensor
	UFUNCTION(BlueprintCallable, Category = "MoCapForKinectV2")
		static bool CloseKinectSensor();

	// Run Kinect update proccess
	UFUNCTION(BlueprintCallable, Category = "MoCapForKinectV2")
		static void RunKinectSensor();

	// Stop Kinect update proccess
	UFUNCTION(BlueprintCallable, Category = "MoCapForKinectV2")
		static void StopKinectSensor();

	// Convert FFaceProperty to FString
	UFUNCTION(BlueprintPure, meta = (BlueprintAutocast, DisplayName = "ToString(FaceProperty)"))
		static FString ConvertFacePropertyToString(FFaceProperty Property);

	// Get parent bone
	UFUNCTION(BlueprintCallable, Category = "MoCapForKinectV2|Joint")
		static EJointType GetParentBoneOfKinectSkeleton(EJointType Joint);

	// Get child bone (Joints with end or multiple connections return JT_Unknown)
	UFUNCTION(BlueprintCallable, Category = "MoCapForKinectV2|Joint")
		static EJointType GetChildBoneOfKinectSkeleton(EJointType Joint);
};
