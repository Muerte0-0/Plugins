// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/VMBoneReference.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "AnimNode_VMRecoil.generated.h"

class URecoilComponent;

// ---------------------------------------------------------------
//  ERecoilAxesOrder
//
//  Controls which skeleton axes receive the recoil component's
//  X/Y/Z output. Match to your rig's bone orientation.
// ---------------------------------------------------------------

UENUM(BlueprintType)
enum class ERecoilAxesOrder : uint8
{
	XYZ UMETA(DisplayName = "XYZ"),
	XZY UMETA(DisplayName = "XZY"),
	YXZ UMETA(DisplayName = "YXZ"),
	YZX UMETA(DisplayName = "YZX"),
	ZXY UMETA(DisplayName = "ZXY"),
	ZYX UMETA(DisplayName = "ZYX"),
};

// ---------------------------------------------------------------
//  FAnimNode_Recoil
//
//  Skeletal control node that reads the recoil transform from
//  URecoilComponent and applies it to a single bone.
//
//  bUseAutoUpdate = true  (default) : the node locates the
//    component automatically via FindComponentByClass in PreUpdate.
//
//  bUseAutoUpdate = false : bind Translation and Rotation pins
//    manually from the Anim Blueprint graph — useful when you want
//    to drive the node from a custom source.
// ---------------------------------------------------------------

USTRUCT(BlueprintInternalUseOnly)
struct FPFRAMEWORK_API FAnimNode_VMRecoil : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "Skeletal Control")
	FBoneReference BoneToModify;

	/** Scale applied to each translation axis after the axes-order remap */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Translation", meta = (PinHiddenByDefault))
	FVector TranslationScale = FVector::OneVector;

	/** Scale applied to each rotation axis after the axes-order remap */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation", meta = (PinHiddenByDefault))
	FVector RotationScale = FVector::OneVector;

	/** Remap the XYZ component output to match this rig's bone axis convention */
	UPROPERTY(EditAnywhere, Category = "Skeletal Control", meta = (PinHiddenByDefault))
	ERecoilAxesOrder AxesOrder = ERecoilAxesOrder::YXZ;

	/** When true the node auto-discovers URecoilComponent on the owning actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Update Mode", meta = (PinShownByDefault))
	bool bUseAutoUpdate = true;

	/** Manual translation input (used when bUseAutoUpdate = false) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Translation", meta = (PinHiddenByDefault))
	FVector Translation = FVector::ZeroVector;

	/** Manual rotation input (used when bUseAutoUpdate = false) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation", meta = (PinHiddenByDefault))
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Category = "Translation")
	TEnumAsByte<EBoneControlSpace> TranslationSpace = BCS_BoneSpace;

	UPROPERTY(EditAnywhere, Category = "Rotation")
	TEnumAsByte<EBoneControlSpace> RotationSpace = BCS_BoneSpace;

	virtual bool HasPreUpdate() const override { return bUseAutoUpdate; }
	virtual void PreUpdate(const UAnimInstance* InAnimInstance) override;
	
protected:
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;

private:
	/** Remap axes and apply scale to produce the final bone-space transform */
	FTransform ComputeRecoilTransform() const;

	bool bIsInitialized = false;

	TWeakObjectPtr<URecoilComponent> RecoilComponent;
	FTransform RecoilTransform = FTransform::Identity;
};
