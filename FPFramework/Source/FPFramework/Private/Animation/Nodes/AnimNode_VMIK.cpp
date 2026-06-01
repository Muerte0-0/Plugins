// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#include "Animation/Nodes/AnimNode_VMIK.h"
#include "AnimationRuntime.h"
#include "Animation/AnimInstanceProxy.h"

void FAnimNode_VMIK::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
    BoneToModify.Initialize(RequiredBones);
}

bool FAnimNode_VMIK::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
    return BoneToModify.IsValidToEvaluate(RequiredBones);
}

void FAnimNode_VMIK::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
    check(OutBoneTransforms.Num() == 0);
    
    const FBoneContainer& BoneContainer = Output.Pose.GetPose().GetBoneContainer();
    
    const FCompactPoseBoneIndex BoneIndex = BoneToModify.GetCompactPoseIndex(BoneContainer);
    if (BoneIndex == INDEX_NONE) return;
    
    // -- Translation Pass -- //

    FTransform BoneTM = Output.Pose.GetComponentSpaceTransform(BoneIndex);
    const FTransform ComponentTM = Output.AnimInstanceProxy->GetComponentTransform();

    FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTM, Output.Pose, BoneTM, BoneIndex, TranslationSpace);

    BoneTM.AddToTranslation(IKMotion.GetLocation());

    FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTM, Output.Pose, BoneTM, BoneIndex, TranslationSpace);
    
    // -- Rotation Pass -- //
    
    FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTM, Output.Pose, BoneTM, BoneIndex, RotationSpace);

    BoneTM.SetRotation(FQuat(IKMotion.GetRotation()) * BoneTM.GetRotation());

    FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTM, Output.Pose, BoneTM, BoneIndex, RotationSpace);

    OutBoneTransforms.Add(FBoneTransform(BoneIndex, BoneTM));
}