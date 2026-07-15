// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "ViewmodelData.generated.h"

USTRUCT(BlueprintType)
struct FPFRAMEWORK_API FVMMovementGaitSettings
{
	GENERATED_BODY()

	// How fast the 0-1 phase clock ticks at this gait
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float PhaseSpeed = 1.0f;

	// Scale applied to the position curve output at this gait
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector PositionScale = FVector(1.0f, 1.0f, 1.0f);

	// Scale applied to the rotation curve output at this gait
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FRotator RotationScale = FRotator(1.0f, 1.0f, 1.0f);

	// Velocity normalisation range — full alpha at this speed (cm/s)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float FullAlphaSpeed = 375.0f;
};

// ============================================================ //
//  UViewmodelData
//  Primary DataAsset — one per weapon/item archetype
// ============================================================ //
UCLASS()
class FPFRAMEWORK_API UViewmodelData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// ---------------------------------------------------------------- //
	// Movement (Procedural Movement Cycles)
	// ------------------------------------------------------------ //

	// -- Position curves -- //

	// Left / Right (cm)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Position Curves")
	TObjectPtr<UCurveFloat> MovementCurve_PosX;

	// Forward / Backward (cm)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Position Curves")
	TObjectPtr<UCurveFloat> MovementCurve_PosY;

	// Up / Down (cm)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Position Curves")
	TObjectPtr<UCurveFloat> MovementCurve_PosZ;

	// -- Rotation curves -- //

	// Pitch (degrees)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Rotation Curves")
	TObjectPtr<UCurveFloat> MovementCurve_Pitch;

	// Yaw (degrees)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Rotation Curves")
	TObjectPtr<UCurveFloat> MovementCurve_Yaw;

	// Roll (degrees)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Rotation Curves")
	TObjectPtr<UCurveFloat> MovementCurve_Roll;

	// Output range remapping per axis — curve 0-1 maps to [Min, Max]
	// This lets the same curve shape serve different magnitude weapons
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Ranges", meta = (UIMin = "-10", UIMax = "10"))
	FVector2D RangePosX = FVector2D(-0.4f, 0.4f);  // cm

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Ranges", meta = (UIMin = "-10", UIMax = "10"))
	FVector2D RangePosY = FVector2D(-0.2f, 0.2f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Ranges", meta = (UIMin = "-10", UIMax = "10"))
	FVector2D RangePosZ = FVector2D(-0.5f, 0.3f);  // asymmetric — drops more than rises

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Ranges", meta = (UIMin = "-15", UIMax = "15"))
	FVector2D RangePitch = FVector2D(-3.0f, 1.5f);  // degrees

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Ranges", meta = (UIMin = "-15", UIMax = "15"))
	FVector2D RangeYaw = FVector2D(-2.0f, 2.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Ranges", meta = (UIMin = "-15", UIMax = "15"))
	FVector2D RangeRoll = FVector2D(-1.6f, 1.6f);

	// Per-gait settings — key: any FGameplayTag the game defines
	// e.g. "Movement.Gait.Walk", "Movement.Gait.Run", "Movement.Gait.Sprint"
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Gait")
	TMap<FGameplayTag, FVMMovementGaitSettings> GaitSettings;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Gait", meta = (UIMin = "1", UIMax = "20"))
	float GaitTransitionSpeed = 6.0f;
	
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Split Hand")
    bool bEnableLeftHand = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Split Hand", meta = (EditCondition = "bEnableLeftHand", UIMin = "0", UIMax = "1"))
    float LeftHandPhaseOffset = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (UIMin = "0", UIMax = "0.5"))
	float CameraMovementScale = 0.15f;

	// Perlin noise — phase jitter and amplitude variation
	// Breaks the mechanical repetition of a perfectly looped cycle
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Noise", meta = (UIMin = "0", UIMax = "0.2"))
	float NoisePhaseScale = 0.04f;   // Max phase jitter (Normalized)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Noise", meta = (UIMin = "0", UIMax = "0.3"))
	float NoiseAmplitudeRange = 0.08f;   // ±amplitude variation (0.08 = ±8%)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Noise", meta = (UIMin = "0", UIMax = "2"))
	float NoiseRotYawScale = 0.3f;    // extra yaw noise (degrees)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Noise", meta = (UIMin = "0", UIMax = "2"))
	float NoiseRotRollScale = 0.5f;    // extra roll noise (degrees)

	// Alpha interp speed — how fast Movement fades in/out when starting/stopping
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (UIMin = "1", UIMax = "30"))
	float AlphaInterpSpeed = 10.0f;

	// Velocity threshold below which alpha is forced to zero (kills idle micro-noise)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (UIMin = "0", UIMax = "0.2"))
	float AlphaDeadzone = 0.05f;

	// Falling alpha multiplier — Movement doesn't stop mid-air, just attenuates
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (UIMin = "0", UIMax = "1"))
	float FallingAlphaScale = 0.25f;
	
	// ------------------------------------------------------------
	//  LAG (Translational inertia)
	// ------------------------------------------------------------
	
	// VInterpTo speed for the main lag vector
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Lag", meta = (UIMin = "1", UIMax = "20"))
	float LagInterpSpeed = 6.0f;

	// Scalar applied before the circular clamp
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Lag", meta = (UIMin = "0", UIMax = "6"))
	float LagVelocityScale = 2.0f;

	// Max magnitude of the lag vector (circular clamp)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Lag", meta = (UIMin = "0", UIMax = "8"))
	float LagMaxMagnitude = 4.0f;

	// Denominator for the vertical lag channel — jump speed of the character
	// Set this to match your character's jump Z velocity (cm/s)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Lag", meta = (UIMin = "100", UIMax = "2000"))
	float LagJumpSpeed = 500.0f;

	// InAirTilt / InAirOffset interp speed (separate — these should feel snappier)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Lag", meta = (UIMin = "1", UIMax = "30"))
	float InAirInterpSpeed = 12.0f;
	
	// ------------------------------------------------------------ //
	//  SWAY (Rotational inertia behind look input)
	// ------------------------------------------------------------ //

	// Spring stiffness — how hard the spring pulls back toward target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sway", meta = (UIMin = "10", UIMax = "300"))
	float SwayStiffness = 120.0f;

	// Exponential damping — prevents infinite oscillation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sway", meta = (UIMin = "1", UIMax = "30"))
	float SwayDamping = 14.0f;

	// Input scale applied to camera delta before computing sway target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sway", meta = (UIMin = "0.5", UIMax = "10"))
	float SwayInputScale = 3.0f;

	// Per-axis clamp on the sway target — X = Pitch (Roll on bone), Y = Yaw
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sway")
	FVector2D SwayMaxAngle = FVector2D(8.0f, 12.0f);

	// ------------------------------------------------------------
	//  Curve Samplers (used by Viewmodel Controller internally)
	// ------------------------------------------------------------

	// Returns curve value remapped from [0,1] to [Range.X, Range.Y]
	// Returns 0 if curve is null — safe to call unconditionally
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
	float SampleMovementPosX(float Phase)  const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
	float SampleMovementPosY(float Phase)  const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
	float SampleMovementPosZ(float Phase)  const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
	float SampleMovementPitch(float Phase) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
	float SampleMovementYaw(float Phase)   const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
	float SampleMovementRoll(float Phase)  const;

	// Returns the gait settings for a tag — falls back to default-constructed if tag not found
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
	FVMMovementGaitSettings GetGaitSettings(FGameplayTag GaitTag) const;

private:

	// Internal helper — avoids repetition in the six sampler functions
	static float SampleCurve(const UCurveFloat* Curve, float Phase, FVector2D Range);
};
