// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BoneContainer.h"
#include "BonePose.h"
#include "Animation/VMBoneReference.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "AnimNode_VMIK.generated.h"

// ---------------------------------------------------------------
//  FAnimNode_VMIK
//
//  Applies an IK motion FTransform (translation + rotation) to a
//  single bone. Driven by FVMAnimState.IKMotion from ViewmodelController.
//
//  Use this for procedural equip, ads, inspect, and landing motions
//  on the weapon/viewmodel bone.
// ---------------------------------------------------------------

USTRUCT(BlueprintInternalUseOnly)
struct FPFRAMEWORK_API FAnimNode_VMIK : public FAnimNode_SkeletalControlBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Skeletal Control")
	FBoneReference BoneToModify;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK Motion", meta = (PinShownByDefault))
	FTransform IKMotion = FTransform::Identity;

	UPROPERTY(EditAnywhere, Category = "IK Motion")
	TEnumAsByte<EBoneControlSpace> TranslationSpace = BCS_ComponentSpace;

	UPROPERTY(EditAnywhere, Category = "IK Motion")
	TEnumAsByte<EBoneControlSpace> RotationSpace = BCS_ComponentSpace;

protected:
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
};