#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimNodeBase.h"
#include "BoneContainer.h"
#include "BonePose.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "BoneControllers/AnimNode_LookAt.h"
#include "MoCapforKinectV2.h"
#include "AnimNode_ApplyKinectJoint.generated.h"

USTRUCT(BlueprintType)
struct MOCAPFORKINECTV2_API FAnimNode_ApplyKinectJoint : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

public:
	// Character bone to apply kinect joint
	UPROPERTY(EditAnywhere, Category = "Modify Target")
		FBoneReference BoneToModify;

	// Bone orientation
	UPROPERTY(EditAnywhere, Category = "Skeletal Control")
		FVector BoneAxis = FVector(1.f, 0.f, 0.f);

	// Joint limmit angle
	UPROPERTY(EditAnywhere, Category = "Skeletal Control", meta = (PinHidenByDefault))
		float JointAngleClamp = 0.f;

	// Rotation sensitivity isn't stable if it's too low, but rotaion isn't reflected much if it's too high
	UPROPERTY(EditAnywhere, Category = "Skeletal Control", meta = (ClampMin = 1, UIMin = 1))
		float Sensitivity = 50.f;

	// The smaller the value, the smoother and later the joint bends (off at 0)
	UPROPERTY(EditAnywhere, Category = "Skeletal Control")
		float InterpSpeed = 15.f;

	// Joint rotation infomation obtained from kinect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target", meta = (PinShownByDefault))
		FRotator JointRotation = FRotator::ZeroRotator;

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
	// LookAt node used internally
	UPROPERTY()
		FAnimNode_LookAt LookAtNode;

	// Delta time updated by UpdateInternal function
	UPROPERTY()
		float DeltaTime = 0.f;

	// Joint orientation in the previous frame
	UPROPERTY()
		FVector BeforeFrameDirection = FVector::ZeroVector;

	// Blend alpha in the previos frame
	UPROPERTY()
		float BeforeFrameAlpha = 0.f;

public:
	// Constructor
	FAnimNode_ApplyKinectJoint();

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

	// Find the direction of bone from rotator
	FVector GetBoneDirection(const USkeletalMeshComponent* SkelMeshComp);

	// Find ainmation blend ratio from tracking state
	float GetBlendAlpha();
};
