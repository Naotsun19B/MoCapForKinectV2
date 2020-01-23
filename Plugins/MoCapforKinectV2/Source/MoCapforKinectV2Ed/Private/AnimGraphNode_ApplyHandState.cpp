// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimGraphNode_ApplyHandState.h"

#define LOCTEXT_NAMESPACE "ApplyHandState"

UAnimGraphNode_ApplyHandState::UAnimGraphNode_ApplyHandState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

FText UAnimGraphNode_ApplyHandState::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if ((TitleType == ENodeTitleType::ListView || TitleType == ENodeTitleType::MenuTitle) && (Node.BoneToModify.BoneName == NAME_None))
	{
		return LOCTEXT("ApplyHandState", "Apply Hand State");
	}
	else
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("ControllerDescription"), LOCTEXT("ApplyHandState", "Apply Hand State"));
		Args.Add(TEXT("BoneName"), FText::FromName(Node.BoneToModify.BoneName));

		if (TitleType == ENodeTitleType::ListView || TitleType == ENodeTitleType::MenuTitle)
		{
			CachedNodeTitles.SetCachedTitle(TitleType, FText::Format(LOCTEXT("AnimGraphNode_ApplyHandState_ListTitle", "{ControllerDescription} - Bone: {BoneName}"), Args), this);
		}
		else
		{
			CachedNodeTitles.SetCachedTitle(TitleType, FText::Format(LOCTEXT("AnimGraphNode_ApplyHandState_Title", "{ControllerDescription}\nBone: {BoneName}"), Args), this);
		}
	}
	return CachedNodeTitles[TitleType];
}
FText UAnimGraphNode_ApplyHandState::GetTooltipText() const
{
	return LOCTEXT("AnimGraphNode_ApplyHandState_Tooltip",
		"Blend hand animation based on hand state obtained from Kinect.\n"
		"All child bones of specified bone will be blended."
	);
}

FLinearColor UAnimGraphNode_ApplyHandState::GetNodeTitleColor() const
{
	return FLinearColor(39.f, 0.f, 117.f);
}

void UAnimGraphNode_ApplyHandState::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	ReconstructNode();
}

#undef LOCTEXT_NAMESPACE
