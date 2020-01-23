// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MoCapforKinectV2.h"
#include "KinectComponent.generated.h"

USTRUCT(BlueprintType)
struct FFaceProperty
{
	GENERATED_USTRUCT_BODY()

public:
	FFaceProperty(EFaceResult happy, EFaceResult engaged, EFaceResult wearingGlasses, EFaceResult leftEyeClosed,
		EFaceResult rightEyeClosed, EFaceResult mouthOpen, EFaceResult mouthMoved, EFaceResult lookingAway)
		: Happy(happy), Engaged(engaged), WearingGlasses(wearingGlasses), LeftEyeClosed(leftEyeClosed),
		RightEyeClosed(rightEyeClosed), MouthOpen(mouthOpen), MouthMoved(mouthMoved), LookingAway(lookingAway) {}

	FFaceProperty() : FFaceProperty(EFaceResult::FR_Unknown, EFaceResult::FR_Unknown, EFaceResult::FR_Unknown, EFaceResult::FR_Unknown,
		EFaceResult::FR_Unknown, EFaceResult::FR_Unknown, EFaceResult::FR_Unknown, EFaceResult::FR_Unknown) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FaceProperty")
		EFaceResult Happy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FaceProperty")
		EFaceResult Engaged;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FaceProperty")
		EFaceResult WearingGlasses;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FaceProperty")
		EFaceResult LeftEyeClosed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FaceProperty")
		EFaceResult RightEyeClosed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FaceProperty")
		EFaceResult MouthOpen;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FaceProperty")
		EFaceResult MouthMoved;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FaceProperty")
		EFaceResult LookingAway;
};

UCLASS( ClassGroup=("MoCapForKinectV2"), meta=(BlueprintSpawnableComponent) )
class MOCAPFORKINECTV2_API UKinectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Person identification number
	UPROPERTY(BlueprintReadWrite, Category = "MoCapForKinectV2", meta = (ClampMax = 5, ClampMin = -1))
		int BodyIndex = -1;

public:	
	// Sets default values for this component's properties
	UKinectComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// Get the state of both hands 
	UFUNCTION(BlueprintCallable, Category = "MoCapForKinectV2|Hand")
		void GetBothHandsState(EHandState& LeftHandState, EHandState& RightHandState);
	
	// Get body tracking status
	UFUNCTION(BlueprintPure, Category = "MoCapForKinectV2|TrackingState")
		bool IsBodyTracking();

	// Get face tracking status
	UFUNCTION(BlueprintPure, Category = "MoCapForKinectV2|TrackingState")
		bool IsFaceTracking();

	// Get tracking status of joint
	UFUNCTION(BlueprintCallable, Category = "MoCapForKinectV2|TrackingState")
		bool GetJointTrackingState(TArray<ETrackingState>& TrackingState);

	// Get location of each joint
	UFUNCTION(BlueprintCallable, Category = "MoCapForKinectV2|Joint")
		bool GetJointLocation(TArray<FVector>& Location);

	// Get rotation of each joint (Doesn't include torsional rotation infomation)
	UFUNCTION(BlueprintCallable, Category = "MoCapForKinectV2|Joint")
		bool GetJointRotation(TArray<FRotator>& Rotation);

	// Get location of root joint
	UFUNCTION(BlueprintCallable, Category = "MoCapForKinectV2|Root")
		bool GetRootLocation(FVector& RootLocation);

	// Get waist twist rotation
	UFUNCTION(BlueprintCallable, Category = "MoCapForKinectV2|Joint")
		bool GetWaistTwist(FRotator& Rotation);

	// Get face property
	UFUNCTION(BlueprintCallable, Category = "MoCapForKinectV2|Face")
		bool GetFaceProperty(FFaceProperty& Property);

	// Get head orientation
	UFUNCTION(BlueprintCallable, Category = "MoCapForKinectV2|Face")
		bool GetFaceRotation(FRotator& Rotation);

	// Getter of body index
	UFUNCTION(BlueprintPure, Category = "MoCapForKinectV2|BodyIndex")
		int GetBodyIndex() const { return BodyIndex; }

	// Setter of body index
	UFUNCTION(BlueprintCallable, Category = "MoCapForKinectV2|BodyIndex")
		void SetBodyIndex(int NewIndex) { BodyIndex = NewIndex; }
};
