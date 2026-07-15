// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#include "GraphNodes/AnimGraphNode_VMRecoil.h"

FText UAnimGraphNode_VMRecoil::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(TEXT("Viewmodel Recoil"));
}

FText UAnimGraphNode_VMRecoil::GetControllerDescription() const
{
	return FText::FromString(TEXT("Viewmodel Recoil Animation"));
}

FText UAnimGraphNode_VMRecoil::GetTooltipText() const
{
	return FText::FromString(
		TEXT("Applies procedural recoil from UVFPS_RecoilComponent to a single bone.\n"
		    "Enable bUseAutoUpdate to auto-discover the component, or drive "
		    "Translation and Rotation pins manually."));
}

FString UAnimGraphNode_VMRecoil::GetNodeCategory() const
{
	return TEXT("Wonderveil Studios|Viewmodel");
}
