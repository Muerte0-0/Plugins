// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveVector.h"
#include "Engine/DataAsset.h"
#include "RecoilData.generated.h"

// ---------------------------------------------------------------
//  Supporting Structs
// ---------------------------------------------------------------

USTRUCT(BlueprintType)
struct FPFRAMEWORK_API FRecoilSway
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil Sway")
	FVector2D PitchSway = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil Sway")
	FVector2D YawSway = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil Sway", meta = (ClampMin = "0.0"))
	float RollSwayMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil Sway", meta = (ClampMin = "0.0"))
	float Damping = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil Sway", meta = (ClampMin = "0.0"))
	float Acceleration = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil Sway", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float AdsScale = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil Sway")
	FVector PivotOffset = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FPFRAMEWORK_API FRecoilProgression
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	float Accel = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	float Damping = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	float Amount = 0.f;
};

USTRUCT(BlueprintType)
struct FPFRAMEWORK_API FControllerRecoilData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Controller Recoil")
	FVector2D HorizontalRecoilStep = FVector2D::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Controller Recoil")
	FVector2D VerticalRecoilStep = FVector2D::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Controller Recoil")
	FVector2D RecoilSmoothing = FVector2D::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Controller Recoil", meta = (ClampMin = "0.0"))
	float Damping = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Controller Recoil", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Compensation = 1.f;
};

// ---------------------------------------------------------------
//  URecoilData
//
//  DataAsset containing all recoil tuning parameters for a single
//  weapon configuration. Assign to UVFPS_RecoilComponent::Init().
//
//  Curve convention:
//    X axis = time
//    Y, Z, W values = per-axis weight in [0, 1]
//  SingleRot / SingleLoc   — used for Semi and first Auto shot
//  AutoRot  / AutoLoc      — used for sustained Auto fire (optional;
//                            falls back to Single curves if null)
// ---------------------------------------------------------------

UCLASS(BlueprintType)
class FPFRAMEWORK_API URecoilData : public UDataAsset
{
	GENERATED_BODY()

public:

	// -----------------------------------------------------------
	//  Controller Recoil
	// -----------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Controller Recoil")
	FControllerRecoilData ControllerRecoil;

	// -----------------------------------------------------------
	//  Curves
	// -----------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curves")
	TObjectPtr<UCurveVector> SingleRot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curves")
	TObjectPtr<UCurveVector> SingleLoc;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curves")
	TObjectPtr<UCurveVector> AutoRot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curves")
	TObjectPtr<UCurveVector> AutoLoc;

	// -----------------------------------------------------------
	//  Input — Rotation (Aiming)
	// -----------------------------------------------------------

	/** Pitch range while aiming.  X = min, Y = max */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Aim")
	FVector2D PitchAim = FVector2D::ZeroVector;

	/** Yaw profile while aiming.  XY = min range, ZW = max range */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Aim")
	FVector4 YawAim = FVector4(0, 0, 0, 0);

	/** Roll profile while aiming.  XY = min range, ZW = max range */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Aim")
	FVector4 RollAim = FVector4(0, 0, 0, 0);

	// -----------------------------------------------------------
	//  Input — Rotation (Hip)
	// -----------------------------------------------------------

	/** Pitch range from hip.  X = min, Y = max */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Hip")
	FVector2D PitchHip = FVector2D::ZeroVector;

	/** Yaw profile from hip.  XY = min range, ZW = max range */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Hip")
	FVector4 YawHip = FVector4(0, 0, 0, 0);

	/** Roll profile from hip.  XY = min range, ZW = max range */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Hip")
	FVector4 RollHip = FVector4(0, 0, 0, 0);

	// -----------------------------------------------------------
	//  Input — Translation (Aiming)
	// -----------------------------------------------------------

	/** Kickback (X) range while aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Aim")
	FVector2D KickAim = FVector2D::ZeroVector;

	/** Kickback right (Y) range while aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Aim")
	FVector2D KickAimR = FVector2D::ZeroVector;

	/** Kickback up (Z) range while aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Aim")
	FVector2D KickAimUp = FVector2D::ZeroVector;

	// -----------------------------------------------------------
	//  Input — Translation (Hip)
	// -----------------------------------------------------------

	/** Kickback (X) range from hip */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Hip")
	FVector2D KickHip = FVector2D::ZeroVector;

	/** Kickback right (Y) range from hip */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Hip")
	FVector2D KickHipR = FVector2D::ZeroVector;

	/** Kickback up (Z) range from hip */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Hip")
	FVector2D KickHipUp = FVector2D::ZeroVector;

	// -----------------------------------------------------------
	//  Auto-Fire Smoothing
	// -----------------------------------------------------------

	/** Per-axis interp speed for rotation in Auto mode. 0 = instant */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Smoothing")
	FRotator SmoothRot = FRotator::ZeroRotator;

	/** Per-axis interp speed for translation in Auto mode. 0 = instant */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Smoothing")
	FVector SmoothLoc = FVector::ZeroVector;

	/** Per-axis magnitude multiplier applied in Auto/Burst mode */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Smoothing")
	FRotator MultiRot = FRotator(1.f, 1.f, 1.f);

	/** Per-axis magnitude multiplier applied in Auto/Burst mode */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Smoothing")
	FVector MultiLoc = FVector(1.f, 1.f, 1.f);

	/** Randomise Roll direction each shot so it alternates sides */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Smoothing")
	bool bSmoothRoll = false;

	// -----------------------------------------------------------
	//  Noise 2D Layer
	// -----------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Noise 2D")
	FVector2D Noise2DTarget = FVector2D::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Noise 2D")
	FVector2D Noise2DAccel = FVector2D::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Noise 2D")
	FVector2D Noise2DZero = FVector2D::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Noise 2D", meta = (ClampMin = "0.0"))
	float NoiseAimScalar = 1.f;

	// -----------------------------------------------------------
	//  Pushback Layer
	// -----------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pushback")
	float Pushback = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pushback")
	float PushbackZero = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pushback")
	float PushbackAccel = 0.f;

	// -----------------------------------------------------------
	//  Progression Layer
	// -----------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Progression")
	FRecoilProgression PitchProgress;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Progression")
	FRecoilProgression ZProgress;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Progression", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ProgressAimAlpha = 0.f;

	// -----------------------------------------------------------
	//  Recoil Sway Layer
	// -----------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil Sway")
	FRecoilSway RecoilSway;

	// -----------------------------------------------------------
	//  Playback
	// -----------------------------------------------------------

	/** Time offset into the curve used to compute the blend start
	 *  value when transitioning from Semi to Auto fire */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Playback")
	float PlaybackOffset = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Playback", meta = (ClampMin = "0.01"))
	float PlayRate = 1.f;

	// -----------------------------------------------------------
	//  Pivot
	// -----------------------------------------------------------

	/** Pivot point applied to the recoil rotation (hip-fire) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pivot")
	FVector PivotOffset = FVector::ZeroVector;

	/** Pivot point applied to the recoil rotation (ADS) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pivot")
	FVector AdsPivotOffset = FVector::ZeroVector;
};
