#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimNodeBase.h"
#include "Animation/AnimNode_SequencePlayer.h"
#include "AnimNode_ApplyHandStateInternal.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct FAnimNode_ApplyHandStateInternal : public FAnimNode_Base
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
		UAnimSequence* Animation_Close;
	UPROPERTY()
		UAnimSequence* Animation_Open;
	UPROPERTY()
		UAnimSequence* Animation_Lasso;

public:
	// The higher the value, the more open
	UPROPERTY()
		float IsOpen = 0.f;
	// The higher the value, the more lasso
	UPROPERTY()
		float IsLasso = 0.f;

private:
	// 3 state animation play node
	UPROPERTY()
		FAnimNode_SequencePlayer AnimNode_Close;
	UPROPERTY()
		FAnimNode_SequencePlayer AnimNode_Open;
	UPROPERTY()
		FAnimNode_SequencePlayer AnimNode_Lasso;

public:
	FAnimNode_ApplyHandStateInternal();

	// FAnimNode_Base interface
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;
	virtual void Evaluate_AnyThread(FPoseContext& Output) override;
	// End of FAnimNode_Base interface
};
