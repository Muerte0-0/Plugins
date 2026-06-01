// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#include "Animation/Nodes/AnimNode_VMMovement.h"
#include "AnimationRuntime.h"
#include "Animation/AnimInstanceProxy.h"

void FAnimNode_VMMovement::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
    PrimaryBoneToModify.Initialize(RequiredBones);

    if (bEnableLeftHand && SecondaryBoneToModify.BoneName != NAME_None)
        SecondaryBoneToModify.Initialize(RequiredBones);
}

bool FAnimNode_VMMovement::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
    return PrimaryBoneToModify.IsValidToEvaluate(RequiredBones);
}

void FAnimNode_VMMovement::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{    
    check(OutBoneTransforms.Num() == 0);
    
    const FBoneContainer& BoneContainer = Output.Pose.GetPose().GetBoneContainer();
    
    const FTransform ComponentTM = Output.AnimInstanceProxy->GetComponentTransform();

    // -- Right Hand -- //
    {
        const FCompactPoseBoneIndex BoneIndex = PrimaryBoneToModify.GetCompactPoseIndex(BoneContainer);
        if (BoneIndex == INDEX_NONE) return;
    
        FVector RemappedPos = FVector::ZeroVector;

        if (EnumHasAnyFlags((EVMAxisFlags)InputX_Axes, EVMAxisFlags::X)) RemappedPos.X += MoveAnimPos.X;
        if (EnumHasAnyFlags((EVMAxisFlags)InputX_Axes, EVMAxisFlags::Y)) RemappedPos.Y += MoveAnimPos.X;
        if (EnumHasAnyFlags((EVMAxisFlags)InputX_Axes, EVMAxisFlags::Z)) RemappedPos.Z += MoveAnimPos.X;

        if (EnumHasAnyFlags((EVMAxisFlags)InputY_Axes, EVMAxisFlags::X)) RemappedPos.X += MoveAnimPos.Y;
        if (EnumHasAnyFlags((EVMAxisFlags)InputY_Axes, EVMAxisFlags::Y)) RemappedPos.Y += MoveAnimPos.Y;
        if (EnumHasAnyFlags((EVMAxisFlags)InputY_Axes, EVMAxisFlags::Z)) RemappedPos.Z += MoveAnimPos.Y;

        if (EnumHasAnyFlags((EVMAxisFlags)InputZ_Axes, EVMAxisFlags::X)) RemappedPos.X += MoveAnimPos.Z;
        if (EnumHasAnyFlags((EVMAxisFlags)InputZ_Axes, EVMAxisFlags::Y)) RemappedPos.Y += MoveAnimPos.Z;
        if (EnumHasAnyFlags((EVMAxisFlags)InputZ_Axes, EVMAxisFlags::Z)) RemappedPos.Z += MoveAnimPos.Z;
        
        FTransform BoneTM = Output.Pose.GetComponentSpaceTransform(BoneIndex);

        FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTM, Output.Pose, BoneTM, BoneIndex, TranslationSpace);
        BoneTM.AddToTranslation(RemappedPos);
        FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTM, Output.Pose, BoneTM, BoneIndex, TranslationSpace);

        FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTM, Output.Pose, BoneTM, BoneIndex, RotationSpace);
        BoneTM.SetRotation(FQuat(MoveAnimRot) * BoneTM.GetRotation());
        FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTM, Output.Pose, BoneTM, BoneIndex, RotationSpace);

        OutBoneTransforms.Add(FBoneTransform(BoneIndex, BoneTM));
    }

    // -- Left Hand (optional) -- //
    if (bEnableLeftHand)
    {
        const FCompactPoseBoneIndex LHBoneIndex = SecondaryBoneToModify.GetCompactPoseIndex(BoneContainer);
        if (LHBoneIndex == INDEX_NONE) return;

        FVector LHRemappedPos = FVector::ZeroVector;

        if (EnumHasAnyFlags((EVMAxisFlags)InputX_Axes, EVMAxisFlags::X)) LHRemappedPos.X += LeftHandMovePos.X;
        if (EnumHasAnyFlags((EVMAxisFlags)InputX_Axes, EVMAxisFlags::Y)) LHRemappedPos.Y += LeftHandMovePos.X;
        if (EnumHasAnyFlags((EVMAxisFlags)InputX_Axes, EVMAxisFlags::Z)) LHRemappedPos.Z += LeftHandMovePos.X;

        if (EnumHasAnyFlags((EVMAxisFlags)InputY_Axes, EVMAxisFlags::X)) LHRemappedPos.X += LeftHandMovePos.Y;
        if (EnumHasAnyFlags((EVMAxisFlags)InputY_Axes, EVMAxisFlags::Y)) LHRemappedPos.Y += LeftHandMovePos.Y;
        if (EnumHasAnyFlags((EVMAxisFlags)InputY_Axes, EVMAxisFlags::Z)) LHRemappedPos.Z += LeftHandMovePos.Y;

        if (EnumHasAnyFlags((EVMAxisFlags)InputZ_Axes, EVMAxisFlags::X)) LHRemappedPos.X += LeftHandMovePos.Z;
        if (EnumHasAnyFlags((EVMAxisFlags)InputZ_Axes, EVMAxisFlags::Y)) LHRemappedPos.Y += LeftHandMovePos.Z;
        if (EnumHasAnyFlags((EVMAxisFlags)InputZ_Axes, EVMAxisFlags::Z)) LHRemappedPos.Z += LeftHandMovePos.Z;

        FTransform LHBoneTM = Output.Pose.GetComponentSpaceTransform(LHBoneIndex);

        FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTM, Output.Pose, LHBoneTM, LHBoneIndex, TranslationSpace);
        LHBoneTM.AddToTranslation(LHRemappedPos);
        FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTM, Output.Pose, LHBoneTM, LHBoneIndex, TranslationSpace);

        FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTM, Output.Pose, LHBoneTM, LHBoneIndex, RotationSpace);
        LHBoneTM.SetRotation(FQuat(LeftHandMoveRot) * LHBoneTM.GetRotation());
        FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTM, Output.Pose, LHBoneTM, LHBoneIndex, RotationSpace);

        OutBoneTransforms.Add(FBoneTransform(LHBoneIndex, LHBoneTM));
    }
}