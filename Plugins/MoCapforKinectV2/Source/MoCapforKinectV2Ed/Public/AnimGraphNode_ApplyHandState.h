// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimGraphNode_Base.h"
#include "AnimNode_ApplyHandState.h"
#include "EdGraphNodeUtils.h"
#include "AnimGraphNode_ApplyHandState.generated.h"

UCLASS(meta = (Keywords = "MoCapForKinectV2"))
class MOCAPFORKINECTV2ED_API UAnimGraphNode_ApplyHandState : public UAnimGraphNode_Base
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Settings")
		FAnimNode_ApplyHandState Node;

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
	// End of UObject interface

private:
	/** Constructing FText strings can be constly, so we cache the node's title **/
	FNodeTitleTextTable CachedNodeTitles;
};
