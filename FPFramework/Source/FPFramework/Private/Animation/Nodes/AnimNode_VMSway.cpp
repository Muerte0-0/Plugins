// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#include "Animation/Nodes/AnimNode_VMSway.h"
#include "AnimationRuntime.h"
#include "Animation/AnimInstanceProxy.h"


void FAnimNode_VMSway::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
    BoneToModify.Initialize(RequiredBones);
}

bool FAnimNode_VMSway::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
    return BoneToModify.IsValidToEvaluate(RequiredBones);
}

void FAnimNode_VMSway::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
    check(OutBoneTransforms.Num() == 0);

    const FBoneContainer& BoneContainer = Output.Pose.GetPose().GetBoneContainer();
    
    const FCompactPoseBoneIndex BoneIndex = BoneToModify.GetCompactPoseIndex(BoneContainer);
    if (BoneIndex == INDEX_NONE) return;
    
    FRotator RemappedRotation = FRotator::ZeroRotator;

    if (EnumHasAnyFlags((EVMAxisFlags)YawAxes,   EVMAxisFlags::X)) RemappedRotation.Roll  += SwayRotation.Yaw;
    if (EnumHasAnyFlags((EVMAxisFlags)YawAxes,   EVMAxisFlags::Y)) RemappedRotation.Pitch += SwayRotation.Yaw;
    if (EnumHasAnyFlags((EVMAxisFlags)YawAxes,   EVMAxisFlags::Z)) RemappedRotation.Yaw   += SwayRotation.Yaw;

    if (EnumHasAnyFlags((EVMAxisFlags)PitchAxes, EVMAxisFlags::X)) RemappedRotation.Roll  += SwayRotation.Pitch;
    if (EnumHasAnyFlags((EVMAxisFlags)PitchAxes, EVMAxisFlags::Y)) RemappedRotation.Pitch += SwayRotation.Pitch;
    if (EnumHasAnyFlags((EVMAxisFlags)PitchAxes, EVMAxisFlags::Z)) RemappedRotation.Yaw   += SwayRotation.Pitch;

    if (EnumHasAnyFlags((EVMAxisFlags)RollAxes,  EVMAxisFlags::X)) RemappedRotation.Roll  += SwayRotation.Roll;
    if (EnumHasAnyFlags((EVMAxisFlags)RollAxes,  EVMAxisFlags::Y)) RemappedRotation.Pitch += SwayRotation.Roll;
    if (EnumHasAnyFlags((EVMAxisFlags)RollAxes,  EVMAxisFlags::Z)) RemappedRotation.Yaw   += SwayRotation.Roll;

    // --- Fetch, convert space, apply, convert back ---

    FTransform BoneTM = Output.Pose.GetComponentSpaceTransform(BoneIndex);
    const FTransform ComponentTM = Output.AnimInstanceProxy->GetComponentTransform();

    FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTM, Output.Pose, BoneTM, BoneIndex, RotationSpace);

    BoneTM.SetRotation(FQuat(RemappedRotation) * BoneTM.GetRotation());

    FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTM, Output.Pose, BoneTM, BoneIndex, RotationSpace);

    OutBoneTransforms.Add(FBoneTransform(BoneIndex, BoneTM));
}