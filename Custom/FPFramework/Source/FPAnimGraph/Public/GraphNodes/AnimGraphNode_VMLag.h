// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimGraphNode_SkeletalControlBase.h"
#include "Animation/Nodes/AnimNode_VMLag.h"
#include "AnimGraphNode_VMLag.generated.h"

/**
 * 
 */
UCLASS()
class FPANIMGRAPH_API UAnimGraphNode_VMLag : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	FAnimNode_VMLag Node;

public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FString GetNodeCategory() const override;

protected:
	virtual FText GetControllerDescription() const override;
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override { return &Node; }
};
