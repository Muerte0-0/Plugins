// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#include "GraphNodes/AnimGraphNode_VMLag.h"

#define LOCTEXT_NAMESPACE "FPAnimGraph"

FText UAnimGraphNode_VMLag::GetControllerDescription() const
{
	return LOCTEXT("VMLagDesc", "Viewmodel Lag");
}
FText UAnimGraphNode_VMLag::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("VMLagTitle", "Viewmodel Lag");
}
FText UAnimGraphNode_VMLag::GetTooltipText() const
{
	return LOCTEXT("VMLagTooltip",
		"Applies procedural Lag translation to a bone, "
		"driven by ViewmodelController.");
}
FString UAnimGraphNode_VMLag::GetNodeCategory() const { return TEXT("Wonderveil Studios|Viewmodel"); }

#undef LOCTEXT_NAMESPACE