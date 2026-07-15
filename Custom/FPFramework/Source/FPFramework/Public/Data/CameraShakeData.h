// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveVector.h"
#include "Engine/DataAsset.h"
#include "CameraShakeData.generated.h"

// ----------------------------------------------------------------------
//  Camera Shake Data
//
//  Data Asset describing one camera shake impulse.
//  Camera Animator plays this back and accumulates up to MaxShakeSlots
//  active instances simultaneously (gunfire + landing, etc.)
//
//  RotationCurve — UCurveVector, 0.Length, values in [-1, 1]
//  The curve is multiplied by a random target sampled from each axis
//  range on PlayCameraShake(), so the same curve asset can produce
//  varied results each time.
// ----------------------------------------------------------------------

UCLASS(BlueprintType)
class FPFRAMEWORK_API UCameraShakeData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    // Normalised shape curve — values should stay in [-1, 1]
    // X = Roll, Y = Pitch, Z = Yaw
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Shake")
    TObjectPtr<UCurveVector> RotationCurve;

    // Random magnitude range for Roll (degrees)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Shake")
    FVector2D Roll = FVector2D(-1.0f, 1.0f);

    // Random magnitude range for Pitch (degrees)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Shake")
    FVector2D Pitch = FVector2D(-1.0f, 1.0f);

    // Random magnitude range for Yaw (degrees)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Shake")
    FVector2D Yaw = FVector2D(-0.5f, 0.5f);

    // VInterpTo smoothing speed applied after curve sampling
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Shake",
    meta = (UIMin = "1", UIMax = "40"))
    float Smoothing = 20.0f;

    // Playback speed multiplier (1.0 = real-time, 2.0 = double speed)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Shake",
    meta = (UIMin = "0.1", UIMax = "10"))
    float PlayRate = 1.0f;
};