// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#include "GraphNodes/AnimGraphNode_VMSway.h"

#define LOCTEXT_NAMESPACE "FPAnimGraph"

FText UAnimGraphNode_VMSway::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("VMSwayTitle", "Viewmodel Sway");
}

FText UAnimGraphNode_VMSway::GetTooltipText() const
{
	return LOCTEXT("VMSwayTooltip", 
		"Applies procedural spring-based sway rotation to a bone, "
		"driven by ViewmodelController look delta.");
}

FString UAnimGraphNode_VMSway::GetNodeCategory() const
{
	return TEXT("Wonderveil Studios|Viewmodel");
}

FText UAnimGraphNode_VMSway::GetControllerDescription() const
{
	return LOCTEXT("VMSwayDesc", "Viewmodel Sway");
}

#undef LOCTEXT_NAMESPACE
