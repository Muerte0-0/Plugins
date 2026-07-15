// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#include "Animation/Nodes/AnimNode_VMLag.h"
#include "AnimationRuntime.h"
#include "Animation/AnimInstanceProxy.h"

void FAnimNode_VMLag::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
    BoneToModify.Initialize(RequiredBones);
}

bool FAnimNode_VMLag::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
    return BoneToModify.IsValidToEvaluate(RequiredBones);
}

void FAnimNode_VMLag::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
    check(OutBoneTransforms.Num() == 0);

    const FBoneContainer& BoneContainer = Output.Pose.GetPose().GetBoneContainer();
    
    const FCompactPoseBoneIndex BoneIndex = BoneToModify.GetCompactPoseIndex(BoneContainer);
    if (BoneIndex == INDEX_NONE) return;
    
    // -- Pass 1 — Location Lag Translation -- //
    
    FTransform BoneTM = Output.Pose.GetComponentSpaceTransform(BoneIndex);
    const FTransform ComponentTM = Output.AnimInstanceProxy->GetComponentTransform();

    FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTM, Output.Pose, BoneTM, BoneIndex, TranslationSpace);
    BoneTM.AddToTranslation(LocationLagPos);
    FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTM, Output.Pose, BoneTM, BoneIndex, TranslationSpace);
    
    // -- Pass 2 — Strafe Tilt -- //
    FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTM, Output.Pose, BoneTM, BoneIndex, TiltSpace);
    BoneTM.AddToTranslation(FVector(0.0f, 0.0f, LocationLagPos.X * TiltTranslationScale));
    const FQuat TiltQuat(FRotator(LocationLagPos.X * TiltRotationScale, 0.0f, 0.0f));
    BoneTM.SetRotation(TiltQuat * BoneTM.GetRotation());
    FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTM, Output.Pose, BoneTM, BoneIndex, TiltSpace);
    
    // -- In Air Offset and Tilt -- //
    FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTM, Output.Pose, BoneTM, BoneIndex, InAirTiltSpace);
    BoneTM.AddToTranslation(InAirOffset);
    BoneTM.SetRotation(FQuat(InAirTilt) * BoneTM.GetRotation());
    FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTM, Output.Pose, BoneTM, BoneIndex, InAirTiltSpace);
    
    OutBoneTransforms.Add(FBoneTransform(BoneIndex, BoneTM));
}