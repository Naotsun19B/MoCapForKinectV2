// Fill out your copyright notice in the Description page of Project Settings.


#include "KinectComponent.h"
#include "KinectAccesser.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

UKinectComponent::UKinectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void UKinectComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UKinectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UKinectComponent::GetBothHandsState(EHandState& LeftHandState, EHandState& RightHandState)
{
	FKinectAccesser* kinectAccesser = FKinectAccesser::GetInstance();
	if (kinectAccesser != NULL)
	{
		kinectAccesser->GetBothHandsState(BodyIndex, LeftHandState, RightHandState);
	}
	else 
	{
		LeftHandState = EHandState::HS_Unknown;
		RightHandState = EHandState::HS_Unknown;
	}
}

bool UKinectComponent::IsBodyTracking()
{
	FKinectAccesser* kinectAccesser = FKinectAccesser::GetInstance();
	if (kinectAccesser != NULL)
	{
		return kinectAccesser->GetBodyTrackingState(BodyIndex);
	}
	else
	{
		return false;
	}
}

bool UKinectComponent::IsFaceTracking()
{
	FKinectAccesser* kinectAccesser = FKinectAccesser::GetInstance();
	if (kinectAccesser != NULL)
	{
		return kinectAccesser->GetFaceTrackingState(BodyIndex);
	}
	else
	{
		return false;
	}
}

bool UKinectComponent::GetJointTrackingState(TArray<ETrackingState>& TrackingState)
{
	FKinectAccesser* kinectAccesser = FKinectAccesser::GetInstance();
	if (kinectAccesser != NULL) 
	{
		TrackingState = kinectAccesser->GetJointTrackingState(BodyIndex);
		return true;
	}
	return false;
}

bool UKinectComponent::GetJointLocation(TArray<FVector>& Location)
{
	FKinectAccesser* kinectAccesser = FKinectAccesser::GetInstance();
	if (kinectAccesser != NULL) 
	{
		Location = kinectAccesser->GetJointLocation(BodyIndex, GetOwner());
		return true;
	}
	return false;
}

bool UKinectComponent::GetJointRotation(TArray<FRotator>& Rotation)
{
	FKinectAccesser* kinectAccesser = FKinectAccesser::GetInstance();
	if (kinectAccesser != NULL) 
	{
		Rotation = kinectAccesser->GetJointRotation(BodyIndex, GetOwner());
		return true;
	}
	return false;
}

bool UKinectComponent::GetRootLocation(FVector& RootLocation)
{
	FKinectAccesser* kinectAccesser = FKinectAccesser::GetInstance();
	if (kinectAccesser != NULL)
	{
		RootLocation = kinectAccesser->GetRootLocation(BodyIndex, GetOwner());
		return true;
	}
	return false;
}

bool UKinectComponent::GetWaistTwist(FRotator& Rotation)
{
	FKinectAccesser* kinectAccesser = FKinectAccesser::GetInstance();
	if (kinectAccesser != NULL)
	{
		Rotation = kinectAccesser->GetWaistTwist(BodyIndex, GetOwner());
		return true;
	}
	return false;
}

bool UKinectComponent::GetFaceProperty(FFaceProperty& Property)
{
	FKinectAccesser* kinectAccesser = FKinectAccesser::GetInstance();
	if (kinectAccesser != NULL) 
	{
		TArray<EFaceResult> data = kinectAccesser->GetFaceProperty(BodyIndex);
		Property.Happy			= data[FaceProperty_Happy];
		Property.Engaged		= data[FaceProperty_Engaged];
		Property.WearingGlasses = data[FaceProperty_WearingGlasses];
		Property.LeftEyeClosed	= data[FaceProperty_LeftEyeClosed];
		Property.RightEyeClosed = data[FaceProperty_RightEyeClosed];
		Property.MouthOpen		= data[FaceProperty_MouthOpen];
		Property.MouthMoved		= data[FaceProperty_MouthMoved];
		Property.LookingAway	= data[FaceProperty_LookingAway];

		return true;
	}
	return false;
}

bool UKinectComponent::GetFaceRotation(FRotator& Rotation)
{
	FKinectAccesser* kinectAccesser = FKinectAccesser::GetInstance();
	if (kinectAccesser != NULL) 
	{
		Rotation = kinectAccesser->GetFaceRotation(BodyIndex, GetOwner());
		return true;
	}
	return false;
}
