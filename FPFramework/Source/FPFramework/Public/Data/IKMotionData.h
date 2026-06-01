// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveVector.h"
#include "Engine/DataAsset.h"
#include "IKMotionData.generated.h"

// ---------------------------------------------------------------
//  UIKMotionData
//
//  DataAsset describing one procedural IK motion — equip, ads-in,
//  ads-out, inspect, landing, etc.
//
//  Both curves are sampled at the current Playback time and scaled
//  per-axis before being combined into FTransform.
//
//  BlendTime - blend-in duration at the START of playback
//  BlendOutTime - blend-out duration at the END of playback
//  Leaving either at 0 snaps immediately (no blend).
// ---------------------------------------------------------------

UCLASS(BlueprintType)
class FPFRAMEWORK_API UIKMotionData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    // Position offset curve (cm) — X/Y/Z channels
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Motion")
    TObjectPtr<UCurveVector> PositionCurve;

    // Rotation curve (degrees) — X=Pitch, Y=Yaw, Z=Roll channels
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Motion")
    TObjectPtr<UCurveVector> RotationCurve;

    // Per-axis position scale multiplier
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Motion")
    FVector PositionScale = FVector::OneVector;

    // Per-axis rotation scale multiplier
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Motion")
    FVector RotationScale = FVector::OneVector;

    // Blend-in time at start of motion (seconds). 0 = instant.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Motion", meta = (UIMin = "0", UIMax = "1"))
    float BlendTime = 0.1f;

    // Blend-out time at end of motion (seconds). 0 = instant snap back to identity.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Motion", meta = (UIMin = "0", UIMax = "1"))
    float BlendOutTime = 0.1f;

    // Playback speed multiplier
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Motion", meta = (UIMin = "0.1", UIMax = "5"))
    float PlayRate = 1.0f;

    // Sample both curves at Time, apply scales, return as FTransform
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "IK Motion")
    FTransform Evaluate(float Time) const;

    // Length of the motion — max of PositionCurve and RotationCurve end times
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "IK Motion")
    float GetLength() const;
};