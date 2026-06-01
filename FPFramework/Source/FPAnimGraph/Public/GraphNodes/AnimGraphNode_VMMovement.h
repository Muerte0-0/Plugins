// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "AnimGraphNode_SkeletalControlBase.h"
#include "Animation/Nodes/AnimNode_VMMovement.h"
#include "AnimGraphNode_VMMovement.generated.h"

UCLASS(MinimalAPI)
class UAnimGraphNode_VMMovement : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Settings")
	FAnimNode_VMMovement Node;

public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FString GetNodeCategory() const override;

protected:
	virtual FText GetControllerDescription() const override;
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override { return &Node; }
};