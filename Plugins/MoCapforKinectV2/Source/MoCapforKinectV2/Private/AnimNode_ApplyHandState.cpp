#include "AnimNode_ApplyHandState.h"
#include "Animation/AnimInstanceProxy.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

FAnimNode_ApplyHandState::FAnimNode_ApplyHandState()
	: InternalNode()
	, LayeredBoneBlendNode()
{

}

void FAnimNode_ApplyHandState::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	FAnimNode_Base::Initialize_AnyThread(Context);
	
	BoneToModify.Initialize(Context.AnimInstanceProxy->GetRequiredBones());

	InternalNode.Animation_Close = Animation_Close;
	InternalNode.Animation_Open = Animation_Open;
	InternalNode.Animation_Lasso = Animation_Lasso;

	InternalNode.Initialize_AnyThread(Context);

	//適用するボーンを設定
	FBranchFilter branchFilter;
	branchFilter.BoneName = BoneToModify.BoneName;
	branchFilter.BlendDepth = 0;
	FInputBlendPose blendPose;
	blendPose.BranchFilters.Add(branchFilter);
	LayeredBoneBlendNode.LayerSetup.Add(blendPose);

	LayeredBoneBlendNode.BasePose = SourcePose;

	//ApplyHandStateInternalノードとlayeredBoneBlendノードを接続
	FPoseLink internalPose;
	internalPose.SetLinkNode(&InternalNode);
	internalPose.Initialize(Context);
	LayeredBoneBlendNode.BlendPoses.Add(internalPose);

	LayeredBoneBlendNode.BlendWeights.Add(0.f);

	LayeredBoneBlendNode.Initialize_AnyThread(Context);
}

void FAnimNode_ApplyHandState::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	LayeredBoneBlendNode.CacheBones_AnyThread(Context);
}

void FAnimNode_ApplyHandState::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	//入力ピンを更新
	GetEvaluateGraphExposedInputs().Execute(Context);

	float isOpen = 0.f;
	float isLasso = 0.f;
	float alpha = 0.f;

	GetHandState(Context.GetDeltaTime(), isOpen, isLasso, alpha);

	InternalNode.IsOpen = isOpen;
	InternalNode.IsLasso = isLasso;
	LayeredBoneBlendNode.BlendWeights[0] = alpha;

	LayeredBoneBlendNode.Update_AnyThread(Context);
}

void FAnimNode_ApplyHandState::Evaluate_AnyThread(FPoseContext& Output)
{
	LayeredBoneBlendNode.Evaluate_AnyThread(Output);
}

void FAnimNode_ApplyHandState::GetHandState(float DeltaTime, float& IsOpen, float& IsLasso, float& Alpha)
{
	float targetIsOpen = (HandState == EHandState::HS_Open ? 1.f : 0.f);
	float targetIsLasso = (HandState == EHandState::HS_Lasso ? 1.f : 0.f);
	float targetAlpha = (HandState == EHandState::HS_NotTracked || HandState == EHandState::HS_Unknown ? 0.f : 1.f);

	IsOpen = FMath::FInterpTo(BeforeFrameIsOpen, targetIsOpen, DeltaTime, InterpSpeed);
	IsLasso = FMath::FInterpTo(BeforeFrameIsLasso, targetIsLasso, DeltaTime, InterpSpeed);
	Alpha = FMath::FInterpTo(BeforeFrameAlpha, targetAlpha, DeltaTime, InterpSpeed);
	
	BeforeFrameIsOpen = IsOpen;
	BeforeFrameIsLasso = IsLasso;
	BeforeFrameAlpha = Alpha;
}
