// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BoneContainer.h"
#include "BonePose.h"

#include "BoneControllers/AnimNode_SkeletalControlBase.h"

#include "Animation/VMAxisFlags.h"
#include "Animation/VMBoneReference.h"

#include "AnimNode_VMMovement.generated.h"

// ---------------------------------------------------------------
//  FAnimNode_VMMovement
//
//  Applies curve-driven Movement translation AND rotation to a bone.
//  Both MoveAnimPos and MoveAnimRot are scaled by MoveAnimAlpha
//  inside the node — no external multiply needed in the graph.
//
//  InputX/Y/Z_Axes control which bone translation axes each
//  position input channel drives (X=Left/Right, Y=Fwd/Bwd, Z=Up/Down).
//  Rotation is applied directly with no remapping — Pitch/Yaw/Roll
//  from MoveAnimRot map straight onto the bone.
// ---------------------------------------------------------------

USTRUCT(BlueprintInternalUseOnly)
struct FPFRAMEWORK_API FAnimNode_VMMovement : public FAnimNode_SkeletalControlBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Skeletal Control")
    FBoneReference PrimaryBoneToModify;

    UPROPERTY(EditAnywhere, Category = "Skeletal Control")
    FBoneReference SecondaryBoneToModify;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (PinShownByDefault))
    FVector MoveAnimPos = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (PinShownByDefault))
    FRotator MoveAnimRot = FRotator::ZeroRotator;
    
    UPROPERTY(EditAnywhere, Category = "Movement")
    TEnumAsByte<EBoneControlSpace> TranslationSpace = BCS_ComponentSpace;

    UPROPERTY(EditAnywhere, Category = "Movement")
    TEnumAsByte<EBoneControlSpace> RotationSpace = BCS_ComponentSpace;

    // Position input channel → bone translation axis mapping
    // Left/Right -> X
    UPROPERTY(EditAnywhere, Category = "Movement", meta = (Bitmask, BitmaskEnum = "/Script/FPFramework.EVMAxisFlags"))
    uint8 InputX_Axes = (uint8)EVMAxisFlags::X;

    // Forward/Backward -> Y
    UPROPERTY(EditAnywhere, Category = "Movement", meta = (Bitmask, BitmaskEnum = "/Script/FPFramework.EVMAxisFlags"))
    uint8 InputY_Axes = (uint8)EVMAxisFlags::Y;

    // Up/Down -> Z
    UPROPERTY(EditAnywhere, Category = "Movement", meta = (Bitmask, BitmaskEnum = "/Script/FPFramework.EVMAxisFlags"))
    uint8 InputZ_Axes = (uint8)EVMAxisFlags::Z;
    
    UPROPERTY(EditAnywhere, Category = "Movement|LeftHand", meta = (PinHiddenByDefault))
    bool bEnableLeftHand = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|LeftHand", meta = (PinHiddenByDefault))
    FVector LeftHandMovePos = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|LeftHand", meta = (PinHiddenByDefault))
    FRotator LeftHandMoveRot = FRotator::ZeroRotator;

protected:
    virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
    virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
    virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
};