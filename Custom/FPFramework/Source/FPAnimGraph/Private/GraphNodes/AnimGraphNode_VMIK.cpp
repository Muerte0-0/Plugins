// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#include "GraphNodes/AnimGraphNode_VMIK.h"

#define LOCTEXT_NAMESPACE "FPAnimGraph"

FText UAnimGraphNode_VMIK::GetControllerDescription() const
{
	return LOCTEXT("VMIKDesc", "Viewmodel IK");
}
FText UAnimGraphNode_VMIK::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("VMIKTitle", "Viewmodel IK");
}
FText UAnimGraphNode_VMIK::GetTooltipText() const
{
	return LOCTEXT("VMIKTooltip",
		"Applies Procedural IK Animation on the Bone. ");
}
FString UAnimGraphNode_VMIK::GetNodeCategory() const { return TEXT("Wonderveil Studios|Viewmodel"); }

#undef LOCTEXT_NAMESPACE