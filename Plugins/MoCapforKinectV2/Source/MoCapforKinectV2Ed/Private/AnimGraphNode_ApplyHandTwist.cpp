// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimGraphNode_ApplyHandTwist.h"

#define LOCTEXT_NAMESPACE "ApplyHandTwist"

UAnimGraphNode_ApplyHandTwist::UAnimGraphNode_ApplyHandTwist(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

FText UAnimGraphNode_ApplyHandTwist::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if ((TitleType == ENodeTitleType::ListView || TitleType == ENodeTitleType::MenuTitle) && (Node.BoneToModify.BoneName == NAME_None))
	{
		return GetControllerDescription();
	}
	else
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("ControllerDescription"), GetControllerDescription());
		Args.Add(TEXT("BoneName"), FText::FromName(Node.BoneToModify.BoneName));

		if (TitleType == ENodeTitleType::ListView || TitleType == ENodeTitleType::MenuTitle)
		{
			CachedNodeTitles.SetCachedTitle(TitleType, FText::Format(LOCTEXT("AnimGraphNode_ApplyHandTwist_ListTitle", "{ControllerDescription} - Bone: {BoneName}"), Args), this);
		}
		else
		{
			CachedNodeTitles.SetCachedTitle(TitleType, FText::Format(LOCTEXT("AnimGraphNode_ApplyHandTwist_Title", "{ControllerDescription}\nBone: {BoneName}"), Args), this);
		}
	}
	return CachedNodeTitles[TitleType];
}

FText UAnimGraphNode_ApplyHandTwist::GetTooltipText() const
{
	return LOCTEXT("AnimGraphNode_ApplyHandTwist_Tooltip",
		"Guess the wrist rotation from the position of the fingertip and thumb that can be obtained from Kinect.\n"
		"The animation blend ratio changes according to the tarcking state.\n"
		"Since the hand information that can be obtained from Kinect is ambiguous,\n"
		"the accuracy of this node can't be expected too much."
	);
}

FLinearColor UAnimGraphNode_ApplyHandTwist::GetNodeTitleColor() const
{
	return FLinearColor(39.f, 0.f, 117.f);
}

void UAnimGraphNode_ApplyHandTwist::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	ReconstructNode();
}

void UAnimGraphNode_ApplyHandTwist::PreEditChange(UProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);
	ReconstructNode();
}

FText UAnimGraphNode_ApplyHandTwist::GetControllerDescription() const
{
	return LOCTEXT("ApplyHandTwist", "Apply Hand Twist");
}

#undef LOCTEXT_NAMESPACE
