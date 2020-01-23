#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimNodeBase.h"
#include "BoneContainer.h"
#include "BonePose.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "BoneControllers/AnimNode_ModifyBone.h"
#include "MoCapforKinectV2.h"
#include "AnimNode_ApplyHandTwist.generated.h"

USTRUCT(BlueprintType)
struct MOCAPFORKINECTV2_API FAnimNode_ApplyHandTwist : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

public:
	// Please specify hand joint
	UPROPERTY(EditAnywhere, Category = "Modify Target")
		FBoneReference BoneToModify;

	// The smaller the value, the smoother and later the joint bends (off at 0)
	UPROPERTY(EditAnywhere, Category = "Skeletal Control")
		float InterpSpeed = 3.f;

	// Thumb joint rotation infomation obtained from kinect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target", meta = (PinShownByDefault))
		FRotator ThumbRotation = FRotator::ZeroRotator;

	// Change the blending ratio of animation according to the tracking state
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alpha", meta = (PinShownByDefault))
		ETrackingState JointTrackingState = ETrackingState::TS_Unknown;

	// The smaller the value, the smoother and later the alpha value is complemented (off at 0)
	UPROPERTY(EditAnywhere, Category = "Alpha")
		float AlphaInterpSpeed = 5.f;

	// Alpha when tracking state is NotTracked
	UPROPERTY(EditAnywhere, Category = "Alpha")
		float NotTracked = 0.f;

	// Alpha when tracking state is Inferred
	UPROPERTY(EditAnywhere, Category = "Alpha")
		float Inferred = 0.5;

	// Alpha when tracking state is Tracked
	UPROPERTY(EditAnywhere, Category = "Alpha")
		float Tracked = 1.f;

protected:
	// Transform (Modify) Bone node used internally
	UPROPERTY()
		FAnimNode_ModifyBone ModifyBoneNode;

	// Delta time updated by UpdateInternal function
	UPROPERTY()
		float DeltaTime = 0.f;

	// Hand rotation in the previous frame
	UPROPERTY()
		FRotator BeforeFrameRotation = FRotator::ZeroRotator;

	// Blend alpha in the previos frame
	UPROPERTY()
		float BeforeFrameAlpha = 0.f;

public:
	// Constructor
	FAnimNode_ApplyHandTwist();

	// FAnimNode_Base interface
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;
	// End of FAnimNode_Base interface

	// FAnimNode_SkeletalControlBase interface
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	virtual void UpdateInternal(const FAnimationUpdateContext& Context) override;
	// End of FAnimNode_SkeletalControlBase interface

private:
	// FAnimNode_SkeletalControlBase interface
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface

	// Get the complemented value of hand rotation
	FRotator GetHandTwist();

	// Find ainmation blend ratio from tracking state
	float GetBlendAlpha();
};
