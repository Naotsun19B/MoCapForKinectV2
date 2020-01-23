// Fill out your copyright notice in the Description page of Project Settings.


#include "KinectBPLibrary.h"
#include "KinectAccesser.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"

int UKinectBPLibrary::GetVaildBodyIndex()
{
	FKinectAccesser* kinectAccesser = FKinectAccesser::GetInstance();
	if (kinectAccesser != NULL) return kinectAccesser->GetVaildBodyIndex();
	else return -2;
}

bool UKinectBPLibrary::GetIsKinectSensorInitialized()
{
	FKinectAccesser* kinectAccesser = FKinectAccesser::GetInstance();
	if (kinectAccesser != NULL) return kinectAccesser->bIsKinectSensorInitialized;
	else return false;
}

bool UKinectBPLibrary::GetIsKinectSensorEnable()
{
	FKinectAccesser* kinectAccesser = FKinectAccesser::GetInstance();
	if (kinectAccesser != NULL) return kinectAccesser->bIsKinectSensorEnable;
	else return false;
}

bool UKinectBPLibrary::InitializeKinectSensor()
{
	FKinectAccesser* kinectAccesser = FKinectAccesser::GetInstance();
	if (kinectAccesser != NULL) return kinectAccesser->InitializeKinectSensor();
	else return false;
}

bool UKinectBPLibrary::CloseKinectSensor()
{
	FKinectAccesser* kinectAccesser = FKinectAccesser::GetInstance();
	if (kinectAccesser != NULL)
	{
		kinectAccesser->CloseKinectSensor();
		return true;
	}
	else return false;
}

void UKinectBPLibrary::RunKinectSensor()
{
	FKinectAccesser* kinectAccesser = FKinectAccesser::GetInstance();
	if (kinectAccesser != NULL && kinectAccesser->bIsKinectSensorInitialized)
		kinectAccesser->bIsKinectSensorEnable = true;
}

void UKinectBPLibrary::StopKinectSensor()
{
	FKinectAccesser* kinectAccesser = FKinectAccesser::GetInstance();
	if (kinectAccesser != NULL && kinectAccesser->bIsKinectSensorInitialized)
		kinectAccesser->bIsKinectSensorEnable = false;
}

FString UKinectBPLibrary::ConvertFacePropertyToString(FFaceProperty Property)
{
	return	"Happy			: " + GETENUMSTRING("EFaceResult", Property.Happy)			+ '\n' +
			"Engaged		: " + GETENUMSTRING("EFaceResult", Property.Engaged)		+ '\n' +
			"WearingGlasses : " + GETENUMSTRING("EFaceResult", Property.WearingGlasses) + '\n' +
			"LeftEyeClosed	: " + GETENUMSTRING("EFaceResult", Property.LeftEyeClosed)	+ '\n' +
			"RightEyeClosed : " + GETENUMSTRING("EFaceResult", Property.RightEyeClosed) + '\n' +
			"MouthOpen		: " + GETENUMSTRING("EFaceResult", Property.MouthOpen)		+ '\n' +
			"MouthMoved		: " + GETENUMSTRING("EFaceResult", Property.MouthMoved)		+ '\n' +
			"LookingAway	: " + GETENUMSTRING("EFaceResult", Property.LookingAway)	+ '\n';
}

EJointType UKinectBPLibrary::GetParentBoneOfKinectSkeleton(EJointType Joint)
{
	switch (Joint)
	{
		case EJointType::JT_SpineBase:		return EJointType::JT_SpineBase;
		case EJointType::JT_SpineMid:		return EJointType::JT_SpineBase;
		case EJointType::JT_Neck:			return EJointType::JT_SpineShoulder;
		case EJointType::JT_Head:			return EJointType::JT_Neck;
		case EJointType::JT_ShoulderLeft:	return EJointType::JT_SpineShoulder;
		case EJointType::JT_ElbowLeft:		return EJointType::JT_ShoulderLeft;
		case EJointType::JT_WristLeft:		return EJointType::JT_ElbowLeft;
		case EJointType::JT_HandLeft:		return EJointType::JT_WristLeft;
		case EJointType::JT_ShoulderRight:	return EJointType::JT_SpineShoulder;
		case EJointType::JT_ElbowRight:		return EJointType::JT_ShoulderRight;
		case EJointType::JT_WristRight:		return EJointType::JT_ElbowRight;
		case EJointType::JT_HandRight:		return EJointType::JT_WristRight;
		case EJointType::JT_HipLeft:		return EJointType::JT_SpineBase;
		case EJointType::JT_KneeLeft:		return EJointType::JT_HipLeft;
		case EJointType::JT_AnkleLeft:		return EJointType::JT_KneeLeft;
		case EJointType::JT_FootLeft:		return EJointType::JT_AnkleLeft;
		case EJointType::JT_HipRight:		return EJointType::JT_SpineBase;
		case EJointType::JT_KneeRight:		return EJointType::JT_HipRight;
		case EJointType::JT_AnkleRight:		return EJointType::JT_KneeRight;
		case EJointType::JT_FootRight:		return EJointType::JT_AnkleRight;
		case EJointType::JT_SpineShoulder:	return EJointType::JT_SpineMid;
		case EJointType::JT_HandTipLeft:	return EJointType::JT_HandLeft;
		case EJointType::JT_ThumbLeft:		return EJointType::JT_HandLeft;
		case EJointType::JT_HandTipRight:	return EJointType::JT_HandRight;
		case EJointType::JT_ThumbRight:		return EJointType::JT_HandRight;
		default:	break;
	}
	return EJointType::JT_Unknown;
}

EJointType UKinectBPLibrary::GetChildBoneOfKinectSkeleton(EJointType Joint)
{
	switch (Joint)
	{
		case EJointType::JT_SpineMid:		return EJointType::JT_SpineShoulder;
		case EJointType::JT_Neck:			return EJointType::JT_Head;
		case EJointType::JT_ShoulderLeft:	return EJointType::JT_ElbowLeft;
		case EJointType::JT_ElbowLeft:		return EJointType::JT_WristLeft;
		case EJointType::JT_WristLeft:		return EJointType::JT_HandLeft;
		case EJointType::JT_HandLeft:		return EJointType::JT_HandTipLeft;
		case EJointType::JT_ShoulderRight:	return EJointType::JT_ElbowRight;
		case EJointType::JT_ElbowRight:		return EJointType::JT_WristRight;
		case EJointType::JT_WristRight:		return EJointType::JT_HandRight;
		case EJointType::JT_HandRight:		return EJointType::JT_HandTipRight;
		case EJointType::JT_HipLeft:		return EJointType::JT_KneeLeft;
		case EJointType::JT_KneeLeft:		return EJointType::JT_AnkleLeft;
		case EJointType::JT_AnkleLeft:		return EJointType::JT_FootLeft;
		case EJointType::JT_HipRight:		return EJointType::JT_KneeRight;
		case EJointType::JT_KneeRight:		return EJointType::JT_AnkleRight;
		case EJointType::JT_AnkleRight:		return EJointType::JT_FootRight;
		case EJointType::JT_SpineShoulder:	return EJointType::JT_Neck;
		default:	break;
	}
	return EJointType::JT_Unknown;
}
