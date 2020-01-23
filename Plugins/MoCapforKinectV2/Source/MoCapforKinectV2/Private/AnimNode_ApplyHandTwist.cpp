#include "AnimNode_ApplyHandTwist.h"
#include "AnimationRuntime.h"
#include "Animation/AnimInstanceProxy.h"

FAnimNode_ApplyHandTwist::FAnimNode_ApplyHandTwist()
	: ModifyBoneNode()
{

}

void FAnimNode_ApplyHandTwist::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	FAnimNode_SkeletalControlBase::Initialize_AnyThread(Context);
	FBoneContainer& requiredBones = Context.AnimInstanceProxy->GetRequiredBones();
	InitializeBoneReferences(requiredBones);
	
	ModifyBoneNode.ComponentPose = ComponentPose;
	ModifyBoneNode.BoneToModify = BoneToModify;
	ModifyBoneNode.BoneToModify.Initialize(Context.AnimInstanceProxy->GetSkeleton());
	ModifyBoneNode.Rotation = GetHandTwist();
	ModifyBoneNode.RotationMode = EBoneModificationMode::BMM_Additive;
	ModifyBoneNode.RotationSpace = EBoneControlSpace::BCS_BoneSpace;

	ModifyBoneNode.Initialize_AnyThread(Context);
}

void FAnimNode_ApplyHandTwist::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	FAnimNode_SkeletalControlBase::CacheBones_AnyThread(Context);
}

void FAnimNode_ApplyHandTwist::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	ModifyBoneNode.EvaluateSkeletalControl_AnyThread(Output, OutBoneTransforms);
}

bool FAnimNode_ApplyHandTwist::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	return BoneToModify.IsValidToEvaluate(RequiredBones);
}

void FAnimNode_ApplyHandTwist::UpdateInternal(const FAnimationUpdateContext& Context)
{
	FAnimNode_SkeletalControlBase::UpdateInternal(Context);

	//ì¸óÕÉsÉìÇçXêV
	GetEvaluateGraphExposedInputs().Execute(Context);

	DeltaTime = Context.GetDeltaTime();

	ModifyBoneNode.Rotation = GetHandTwist();

	Alpha = GetBlendAlpha();
	ModifyBoneNode.Alpha = Alpha;
}

void FAnimNode_ApplyHandTwist::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	BoneToModify.Initialize(RequiredBones);
}

FRotator FAnimNode_ApplyHandTwist::GetHandTwist()
{
	FRotator target = FRotator(ThumbRotation.Pitch * -1 - 90, 0.f, 0.f);
	FRotator result = FMath::RInterpTo(BeforeFrameRotation, target, DeltaTime, InterpSpeed);

	BeforeFrameRotation = result;
	
	return result;
}

float FAnimNode_ApplyHandTwist::GetBlendAlpha()
{
	float target = 0.f;

	if (JointTrackingState == ETrackingState::TS_Tracked)
	{
		target = Tracked;
	}
	else if (JointTrackingState == ETrackingState::TS_Inferred)
	{
		target = Inferred;
	}
	else
	{
		target = NotTracked;
	}

	float result = FMath::FInterpTo(Alpha, target, DeltaTime, AlphaInterpSpeed);

	BeforeFrameAlpha = result;

	return result;
}
