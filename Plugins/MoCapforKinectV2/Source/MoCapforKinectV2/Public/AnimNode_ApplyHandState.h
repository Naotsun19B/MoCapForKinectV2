#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimNodeBase.h"
#include "Animation/AnimNode_SequencePlayer.h"
#include "MoCapforKinectV2/Private/AnimNode_ApplyHandStateInternal.h"
#include "AnimNode_LayeredBoneBlend.h"
#include "MoCapforKinectV2.h"
#include "AnimNode_ApplyHandState.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct MOCAPFORKINECTV2_API FAnimNode_ApplyHandState : public FAnimNode_Base
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Links")
		FPoseLink SourcePose;

	// All child bones of specified bone will ble blended (Please specify wrist bone)
	UPROPERTY(EditAnywhere, Category = "Modify Target")
		FBoneReference BoneToModify;

	// Hand state information obtained from Kinect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Target, meta = (PinShownByDefault))
		EHandState HandState = EHandState::HS_Unknown;

	// Closed hand (Rock of Rock-Paper-Scissors) animation asset
	UPROPERTY(EditAnywhere, Category = "Animation Asset")
		UAnimSequence* Animation_Close;

	// Opened hand (Paper of Rock-Paper-Scissors) animation asset
	UPROPERTY(EditAnywhere, Category = "Animation Asset")
		UAnimSequence* Animation_Open;

	// Lasso hand (Scissors of Rock-Paper-Scissors) animation asset
	UPROPERTY(EditAnywhere, Category = "Animation Asset")
		UAnimSequence* Animation_Lasso;

	// The smaller the value, the smoother and later the hand state bends (off at 0)
	UPROPERTY(EditAnywhere, Category = "Alpha")
		float InterpSpeed = 0.f;

private:
	// Node for internal processing
	UPROPERTY()
		FAnimNode_ApplyHandStateInternal InternalNode;

	// LayeredBoneBlend node used internally
	UPROPERTY()
		FAnimNode_LayeredBoneBlend LayeredBoneBlendNode;

	// Blend alpha values in the previos frame
	UPROPERTY()
		float BeforeFrameIsOpen = 0.f;
	UPROPERTY()
		float BeforeFrameIsLasso = 0.f;
	UPROPERTY()
		float BeforeFrameAlpha = 0.f;

public:
	// Constructor
	FAnimNode_ApplyHandState();

	// FAnimNode_Base interface
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;
	virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;
	virtual void Evaluate_AnyThread(FPoseContext& Output) override;
	// End of FAnimNode_Base interface

private:
	// Find multiple alpha values that determine hand state from EHandState
	void GetHandState(float DeltaTime, float& IsOpen, float& IsLasso, float& Alpha);
};
