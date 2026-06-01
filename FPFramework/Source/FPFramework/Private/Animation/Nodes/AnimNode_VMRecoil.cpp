// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#include "Animation/Nodes/AnimNode_VMRecoil.h"

#include "Animation/AnimInstanceProxy.h"
#include "Animation/AnimInstance.h"
#include "Components/RecoilComponent.h"

FTransform FAnimNode_VMRecoil::ComputeRecoilTransform() const
{
	FTransform Final = bUseAutoUpdate ? RecoilTransform : FTransform(Rotation.Quaternion(), Translation);

	const FVector  Loc = Final.GetLocation();
	const FRotator Rot = Final.Rotator();

	switch (AxesOrder)
	{
	case ERecoilAxesOrder::XZY:
		Final.SetLocation(FVector(Loc.X, Loc.Z, Loc.Y));
		Final.SetRotation(FRotator(Rot.Pitch, Rot.Roll, Rot.Yaw).Quaternion());
		break;

	case ERecoilAxesOrder::YXZ:
		Final.SetLocation(FVector(Loc.Y, Loc.X, Loc.Z));
		Final.SetRotation(FRotator(Rot.Roll, Rot.Yaw, Rot.Pitch).Quaternion());
		break;

	case ERecoilAxesOrder::YZX:
		Final.SetLocation(FVector(Loc.Y, Loc.Z, Loc.X));
		Final.SetRotation(FRotator(Rot.Roll, Rot.Pitch, Rot.Yaw).Quaternion());
		break;

	case ERecoilAxesOrder::ZXY:
		Final.SetLocation(FVector(Loc.Z, Loc.X, Loc.Y));
		Final.SetRotation(FRotator(Rot.Yaw, Rot.Pitch, Rot.Roll).Quaternion());
		break;

	case ERecoilAxesOrder::ZYX:
		Final.SetLocation(FVector(Loc.Z, Loc.Y, Loc.X));
		Final.SetRotation(FRotator(Rot.Yaw, Rot.Roll, Rot.Pitch).Quaternion());
		break;

	default: // XYZ — no remap needed
		break;
	}

	return Final;
}

// ---------------------------------------------------------------
//  PreUpdate — runs on the game thread before EvaluateAnyThread.
//  Auto-discovers the recoil component on first call.
// ---------------------------------------------------------------

void FAnimNode_VMRecoil::PreUpdate(const UAnimInstance* InAnimInstance)
{
	if (!bIsInitialized)
	{
		if (AActor* Owner = InAnimInstance->GetOwningActor())
		{
			// Wait until BeginPlay has run so the component is fully set up
			if (Owner->HasActorBegunPlay())
			{
				RecoilComponent = Owner->FindComponentByClass<URecoilComponent>();
				bIsInitialized  = true;
			}
		}
	}

	if (RecoilComponent.IsValid())
	{
		RecoilTransform = RecoilComponent->GetOutput();
	}
}

// ---------------------------------------------------------------
//  EvaluateSkeletalControl_AnyThread
//  Applies the finalised recoil transform to BoneToModify in
//  component space, then converts back.
// ---------------------------------------------------------------

void FAnimNode_VMRecoil::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	check(OutBoneTransforms.Num() == 0);

	const FBoneContainer& BoneContainer = Output.Pose.GetPose().GetBoneContainer();
	
	const FCompactPoseBoneIndex BoneIndex = BoneToModify.GetCompactPoseIndex(BoneContainer);

	FTransform NewBoneTM = Output.Pose.GetComponentSpaceTransform(BoneIndex);
	FTransform ComponentTM = Output.AnimInstanceProxy->GetComponentTransform();

	// --- Rotation ---
	FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTM, Output.Pose, NewBoneTM, BoneIndex, RotationSpace);

	FTransform Final = ComputeRecoilTransform();
	FVector FinalPosition = Final.GetTranslation();
	FRotator FinalRotation = Final.Rotator();

	FinalPosition.X *= TranslationScale.X;
	FinalPosition.Y *= TranslationScale.Y;
	FinalPosition.Z *= TranslationScale.Z;

	FinalRotation.Pitch *= RotationScale.Y;
	FinalRotation.Yaw *= RotationScale.Z;
	FinalRotation.Roll *= RotationScale.X;

	Final.SetTranslation(FinalPosition);
	Final.SetRotation(FinalRotation.Quaternion());

	NewBoneTM.SetRotation(Final.GetRotation() * NewBoneTM.GetRotation());

	FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTM, Output.Pose, NewBoneTM, BoneIndex, RotationSpace);

	// --- Translation ---
	FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTM, Output.Pose, NewBoneTM, BoneIndex, TranslationSpace);

	NewBoneTM.AddToTranslation(Final.GetTranslation());

	FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTM, Output.Pose, NewBoneTM, BoneIndex, TranslationSpace);

	OutBoneTransforms.Add(FBoneTransform(BoneIndex, NewBoneTM));
}

// ---------------------------------------------------------------
//  IsValidToEvaluate / InitializeBoneReferences
// ---------------------------------------------------------------

bool FAnimNode_VMRecoil::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	return BoneToModify.IsValidToEvaluate(RequiredBones);
}

void FAnimNode_VMRecoil::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(InitializeBoneReferences)
	BoneToModify.Initialize(RequiredBones);
}
