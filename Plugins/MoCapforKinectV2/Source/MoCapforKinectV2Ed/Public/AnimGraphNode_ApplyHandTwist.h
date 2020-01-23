// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimGraphNode_SkeletalControlBase.h"
#include "AnimNode_ApplyHandTwist.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "AnimGraphNode_ApplyHandTwist.generated.h"

UCLASS(meta = (Keywords = "MoCapForKinectV2"))
class MOCAPFORKINECTV2ED_API UAnimGraphNode_ApplyHandTwist : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Settings")
		FAnimNode_ApplyHandTwist Node;

public:
	// UK2Node_FunctionEntry interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	// End of UK2Node_FunctionEntry interface

	// UEdGraphNode interface
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	// End of UEdGraphNode interface

	// UObject interface
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PreEditChange(UProperty* PropertyAboutToChange) override;
	// End of UObject interface

protected:
	// UAnimGraphNode_SkeletalControlBase interface
	virtual FText GetControllerDescription() const override;
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override { return &Node; }
	// End of UAnimGraphNode_SkeletalControlBase interface

private:
	/** Constructing FText strings can be constly, so we cache the node's title **/
	FNodeTitleTextTable CachedNodeTitles;
};
