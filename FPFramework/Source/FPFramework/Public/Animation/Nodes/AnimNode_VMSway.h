// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BoneContainer.h"
#include "BonePose.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "Animation/VMAxisFlags.h"
#include "Animation/VMBoneReference.h"
#include "AnimNode_VMSway.generated.h"


// ---------------------------------------------------------------
//  FAnimNode_VMSway
//
//  Applies a rotational offset (sway) to a single bone.
//  Input: FRotator driven by UViewmodelController::SwayRotation.
//
//  YawAxes   — which bone rotation axes the input Yaw channel drives
//  PitchAxes — which bone rotation axes the input Pitch channel drives
//  RollAxes  — which bone rotation axes the input Roll channel drives
//  (Use multi-select to fan one channel onto several axes)
// ---------------------------------------------------------------

USTRUCT(BlueprintInternalUseOnly)
struct FPFRAMEWORK_API FAnimNode_VMSway : public FAnimNode_SkeletalControlBase
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, Category = "Skeletal Control")
    FBoneReference BoneToModify;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sway", meta = (PinShownByDefault))
    FRotator SwayRotation = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, Category = "Sway")
    TEnumAsByte<EBoneControlSpace> RotationSpace = BCS_ComponentSpace;

    // Input channel → output bone axis mapping
    UPROPERTY(EditAnywhere, Category = "Sway|AxisMapping", meta = (Bitmask, BitmaskEnum = "/Script/FPFramework.EVMAxisFlags"))
    uint8 YawAxes = (uint8)EVMAxisFlags::Z;

    UPROPERTY(EditAnywhere, Category = "Sway|AxisMapping", meta = (Bitmask, BitmaskEnum = "/Script/FPFramework.EVMAxisFlags"))
    uint8 PitchAxes = (uint8)EVMAxisFlags::Y;

    UPROPERTY(EditAnywhere, Category = "Sway|AxisMapping", meta = (Bitmask, BitmaskEnum = "/Script/FPFramework.EVMAxisFlags"))
    uint8 RollAxes = (uint8)EVMAxisFlags::X;

protected:
    virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
    virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
    virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
};