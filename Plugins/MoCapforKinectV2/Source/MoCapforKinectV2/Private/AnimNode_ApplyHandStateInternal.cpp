#include "AnimNode_ApplyHandStateInternal.h"
#include "Animation/AnimInstanceProxy.h"
#include "AnimationRuntime.h"
#include "MoCapForKinectV2.h"

FAnimNode_ApplyHandStateInternal::FAnimNode_ApplyHandStateInternal()
	: AnimNode_Close()
	, AnimNode_Open()
	, AnimNode_Lasso()
{

}

void FAnimNode_ApplyHandStateInternal::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	FAnimNode_Base::Initialize_AnyThread(Context);
	AnimNode_Close.Initialize_AnyThread(Context);
	AnimNode_Open.Initialize_AnyThread(Context);
	AnimNode_Lasso.Initialize_AnyThread(Context);
	AnimNode_Close.Sequence = Animation_Close;
	AnimNode_Open.Sequence = Animation_Open;
	AnimNode_Lasso.Sequence = Animation_Lasso;
}

void FAnimNode_ApplyHandStateInternal::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	if (Animation_Close == nullptr || Animation_Open == nullptr || Animation_Lasso == nullptr)
	{
		UE_LOG(MoCapForKinectV2, Warning, TEXT("Please specify all animation assets in ApplyHandState."));
		return;
	}

	AnimNode_Close.UpdateAssetPlayer(Context);
	AnimNode_Open.UpdateAssetPlayer(Context);
	AnimNode_Lasso.UpdateAssetPlayer(Context);
}

void FAnimNode_ApplyHandStateInternal::Evaluate_AnyThread(FPoseContext& Output)
{
	if (Animation_Close == nullptr || Animation_Open == nullptr || Animation_Lasso == nullptr)
	{
		UE_LOG(MoCapForKinectV2, Warning, TEXT("Please specify all animation assets in ApplyHandState."));
		return;
	}

	const FBoneContainer* RequiredBones = &Output.AnimInstanceProxy->GetRequiredBones();

	//���͂���|�[�Y�p�̕ϐ���������
	FCompactPose pose_Close;
	FCompactPose pose_Open;
	FCompactPose pose_Lasso;
	pose_Close.SetBoneContainer(RequiredBones);
	pose_Open.SetBoneContainer(RequiredBones);
	pose_Lasso.SetBoneContainer(RequiredBones);

	//���͂���J�[�u�p�̕ϐ���������
	FBlendedCurve curve_Close;
	FBlendedCurve curve_Open;
	FBlendedCurve curve_Lasso;
	curve_Close.InitFrom(*RequiredBones);
	curve_Open.InitFrom(*RequiredBones);
	curve_Lasso.InitFrom(*RequiredBones);

	//�A�j���[�V�����̍Đ����Ԃ⃋�[�g���[�V�������g�����̕ϐ���������
	FAnimExtractContext context_Close(AnimNode_Close.GetAccumulatedTime(), Animation_Close->bEnableRootMotion);
	FAnimExtractContext context_Open(AnimNode_Open.GetAccumulatedTime(), Animation_Open->bEnableRootMotion);
	FAnimExtractContext context_Lasso(AnimNode_Lasso.GetAccumulatedTime(), Animation_Lasso->bEnableRootMotion);

	//�o�͗p�̃|�[�Y�ƃJ�[�u�p�̕ϐ���������
	FCompactPose pose_Close_Open;
	FBlendedCurve curve_Close_Open;
	pose_Close_Open.SetBoneContainer(RequiredBones);
	curve_Close_Open.InitFrom(*RequiredBones);

	//�A�j���[�V�����̍Đ����Ԃ���|�[�Y�ƃJ�[�u���擾
	Animation_Close->GetBonePose(pose_Close, curve_Close, context_Close);
	Animation_Open->GetBonePose(pose_Open, curve_Open, context_Open);
	Animation_Lasso->GetBonePose(pose_Lasso, curve_Lasso, context_Lasso);

	//��̃A�j���[�V�������u�����h
	FAnimationRuntime::BlendTwoPosesTogether(
		pose_Close, pose_Open, curve_Close, curve_Open, 1.f - IsOpen, pose_Close_Open, curve_Close_Open
	);

	//�P��ڂ̌��ʂƎO�ڂ̃A�j���[�V�������u�����h
	FAnimationRuntime::BlendTwoPosesTogether(
		pose_Close_Open, pose_Lasso, curve_Close_Open, curve_Lasso, 1.f - IsLasso, Output.Pose, Output.Curve
	);
}
