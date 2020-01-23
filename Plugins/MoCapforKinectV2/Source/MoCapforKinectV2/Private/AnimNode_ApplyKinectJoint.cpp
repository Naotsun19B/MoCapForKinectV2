#include "AnimNode_ApplyKinectJoint.h"
#include "AnimationRuntime.h"
#include "Animation/AnimInstanceProxy.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

FAnimNode_ApplyKinectJoint::FAnimNode_ApplyKinectJoint()
	: LookAtNode()
{

}

void FAnimNode_ApplyKinectJoint::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	FAnimNode_SkeletalControlBase::Initialize_AnyThread(Context);
	FBoneContainer& requiredBones = Context.AnimInstanceProxy->GetRequiredBones();
	InitializeBoneReferences(requiredBones);

	LookAtNode.BoneToModify = BoneToModify;
	LookAtNode.LookAtClamp = JointAngleClamp;
	LookAtNode.LookAtLocation = GetBoneDirection(Context.AnimInstanceProxy->GetSkelMeshComponent());
	
	if (BoneAxis == FVector::ZeroVector)
	{
		UE_LOG(MoCapForKinectV2, Warning, TEXT("Zero-length bone axis specified in ApplyKinectJoint node. Reverting to default."));
		LookAtNode.LookAt_Axis = FVector(1.f, 0.f, 0.f);
	}
	else
	{
		LookAtNode.LookAt_Axis = BoneAxis;
	}

	LookAtNode.Initialize_AnyThread(Context);
}

void FAnimNode_ApplyKinectJoint::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	FAnimNode_SkeletalControlBase::CacheBones_AnyThread(Context);
	LookAtNode.CacheBones_AnyThread(Context);
}

void FAnimNode_ApplyKinectJoint::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	LookAtNode.EvaluateSkeletalControl_AnyThread(Output, OutBoneTransforms);
}

bool FAnimNode_ApplyKinectJoint::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	return BoneToModify.IsValidToEvaluate(RequiredBones);
}

void FAnimNode_ApplyKinectJoint::UpdateInternal(const FAnimationUpdateContext& Context)
{
	FAnimNode_SkeletalControlBase::UpdateInternal(Context);

	//“ü—Íƒsƒ“‚ðXV
	GetEvaluateGraphExposedInputs().Execute(Context);

	DeltaTime = Context.GetDeltaTime();
	LookAtNode.LookAtLocation = GetBoneDirection(Context.AnimInstanceProxy->GetSkelMeshComponent());

	Alpha = GetBlendAlpha();
	LookAtNode.Alpha = Alpha;

	LookAtNode.UpdateInternal(Context);
}

void FAnimNode_ApplyKinectJoint::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	BoneToModify.Initialize(RequiredBones);
}

FVector FAnimNode_ApplyKinectJoint::GetBoneDirection(const USkeletalMeshComponent* SkelMeshComp)
{
	FVector boneLocation = SkelMeshComp->GetSocketLocation(BoneToModify.BoneName);
	FVector boneDirection = UKismetMathLibrary::GetForwardVector(JointRotation);
	FVector target = boneLocation + boneDirection * Sensitivity;
	
	FVector result = FMath::VInterpTo(BeforeFrameDirection, target, DeltaTime, InterpSpeed);
	BeforeFrameDirection = result;

	return result;
}

float FAnimNode_ApplyKinectJoint::GetBlendAlpha()
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
