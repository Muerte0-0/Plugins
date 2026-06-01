// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BoneContainer.h"
#include "BonePose.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "Animation/VMBoneReference.h"
#include "AnimNode_VMLag.generated.h"


// ---------------------------------------------------------------
//  FAnimNode_VMLag
//
//  Applies translational lag (LocationLagPos) to BoneToModify.
//
//  Tilt pass (optional, same or different bone):
//    TiltBone — receives InAirOffset (translation) and InAirTilt (rotation)
//    This replaces the separate "Modify Bone" node that was previously
//    needed to derive tilt from the vertical lag channel.
//
//  TiltTranslationScale and TiltRotationScale match the values you
//  were using: X * 0.5 → Z translation, X * 5 → Pitch rotation.
//  These are now configurable per DataAsset archetype.
// ---------------------------------------------------------------

USTRUCT(BlueprintInternalUseOnly)
struct FPFRAMEWORK_API FAnimNode_VMLag : public FAnimNode_SkeletalControlBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Skeletal Control")
    FBoneReference BoneToModify;

    // Channels: X = Right/Left, Y = Fwd/Bwd, Z = Up/Down (vertical)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lag", meta = (PinShownByDefault))
    FVector LocationLagPos = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, Category = "Lag")
    TEnumAsByte<EBoneControlSpace> TranslationSpace = BCS_ComponentSpace;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lag", meta = (UIMin = "-5", UIMax = "5"))
    float TiltTranslationScale = 0.5f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lag", meta = (UIMin = "-20", UIMax = "20"))
    float TiltRotationScale = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Lag")
    TEnumAsByte<EBoneControlSpace> TiltSpace = BCS_ComponentSpace;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lag", meta = (PinShownByDefault))
    FVector InAirOffset = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lag", meta = (PinShownByDefault))
    FRotator InAirTilt = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, Category = "Lag")
    TEnumAsByte<EBoneControlSpace> InAirTiltSpace = BCS_ComponentSpace;

protected:
    virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
    virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
    virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
};