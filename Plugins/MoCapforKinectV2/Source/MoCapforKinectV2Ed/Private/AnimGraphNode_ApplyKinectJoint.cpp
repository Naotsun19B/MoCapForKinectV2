// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimGraphNode_ApplyKinectJoint.h"

#define LOCTEXT_NAMESPACE "ApplyKinectJoint"

UAnimGraphNode_ApplyKinectJoint::UAnimGraphNode_ApplyKinectJoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

FText UAnimGraphNode_ApplyKinectJoint::GetNodeTitle(ENodeTitleType::Type TitleType) const
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
			CachedNodeTitles.SetCachedTitle(TitleType, FText::Format(LOCTEXT("AnimGraphNode_ApplyKinectJoint_ListTitle", "{ControllerDescription} - Bone: {BoneName}"), Args), this);
		}
		else
		{
			CachedNodeTitles.SetCachedTitle(TitleType, FText::Format(LOCTEXT("AnimGraphNode_ApplyKinectJoint_Title", "{ControllerDescription}\nBone: {BoneName}"), Args), this);
		}
	}
	return CachedNodeTitles[TitleType];
}

FText UAnimGraphNode_ApplyKinectJoint::GetTooltipText() const
{
	return LOCTEXT("AnimGraphNode_ApplyKinectJoint_Tooltip", 
		"Rotate the specified bone based on the joint and tracking infomation obtained from Kinect.\n"
		"The animation blend ratio changes according to the tarcking state."
	);
}

FLinearColor UAnimGraphNode_ApplyKinectJoint::GetNodeTitleColor() const
{
	return FLinearColor(39.f, 0.f, 117.f);
}

void UAnimGraphNode_ApplyKinectJoint::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	ReconstructNode();
}

void UAnimGraphNode_ApplyKinectJoint::PreEditChange(UProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);
	ReconstructNode();
}

FText UAnimGraphNode_ApplyKinectJoint::GetControllerDescription() const
{
	return LOCTEXT("ApplyKinectJoint", "Apply Kinect Joint");
}

#undef LOCTEXT_NAMESPACE
