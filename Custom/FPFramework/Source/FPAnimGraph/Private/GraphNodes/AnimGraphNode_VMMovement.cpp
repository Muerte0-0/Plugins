// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#include "GraphNodes/AnimGraphNode_VMMovement.h"

#define LOCTEXT_NAMESPACE "FPAnimGraph"

FText UAnimGraphNode_VMMovement::GetControllerDescription() const
{
	return LOCTEXT("VMMovementDesc", "Viewmodel Movement");
}
FText UAnimGraphNode_VMMovement::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("VMMovementTitle", "Viewmodel Movement");
}
FText UAnimGraphNode_VMMovement::GetTooltipText() const
{
	return LOCTEXT("VMMovementTooltip",
		"Applies procedural sine-based Movement translation to a bone, "
		"driven by ViewmodelController movement speed.");
}
FString UAnimGraphNode_VMMovement::GetNodeCategory() const { return TEXT("Wonderveil Studios|Viewmodel"); }

#undef LOCTEXT_NAMESPACE