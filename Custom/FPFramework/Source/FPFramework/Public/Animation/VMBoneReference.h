// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "VMBoneReference.generated.h"

// ---------------------------------------------------------------
//  FVMBoneReference
//
//  Blueprint-exposed bone selector used by Viewmodel Anim nodes.
// ---------------------------------------------------------------

USTRUCT(BlueprintType)
struct FPFRAMEWORK_API FVMBoneReference
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Bone")
	FBoneReference Bone;
};