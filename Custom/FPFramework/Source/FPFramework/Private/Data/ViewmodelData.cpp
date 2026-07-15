// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#include "Data/ViewmodelData.h"

// ---------------------------------------------------------------
//  Internal helper
// ---------------------------------------------------------------

float UViewmodelData::SampleCurve(const UCurveFloat* Curve, float Phase, FVector2D Range)
{
	if (!Curve) return 0.0f;
	return FMath::Lerp(Range.X, Range.Y, Curve->GetFloatValue(Phase));
}


// ---------------------------------------------------------------
//  Curve samplers
// ---------------------------------------------------------------

float UViewmodelData::SampleMovementPosX(float Phase)  const { return SampleCurve(MovementCurve_PosX,  Phase, RangePosX);  }
float UViewmodelData::SampleMovementPosY(float Phase)  const { return SampleCurve(MovementCurve_PosY,  Phase, RangePosY);  }
float UViewmodelData::SampleMovementPosZ(float Phase)  const { return SampleCurve(MovementCurve_PosZ,  Phase, RangePosZ);  }
float UViewmodelData::SampleMovementPitch(float Phase) const { return SampleCurve(MovementCurve_Pitch, Phase, RangePitch); }
float UViewmodelData::SampleMovementYaw(float Phase)   const { return SampleCurve(MovementCurve_Yaw,   Phase, RangeYaw);   }
float UViewmodelData::SampleMovementRoll(float Phase)  const { return SampleCurve(MovementCurve_Roll,  Phase, RangeRoll);  }


// ---------------------------------------------------------------
//  Gait lookup — safe fallback if tag not found
// ---------------------------------------------------------------

FVMMovementGaitSettings UViewmodelData::GetGaitSettings(FGameplayTag GaitTag) const
{
	if (const FVMMovementGaitSettings* Found = GaitSettings.Find(GaitTag))
		return *Found;

	UE_LOG(LogTemp, Warning, TEXT("UViewmodelData: GaitTag '%s' not found in GaitSettings on '%s' — using defaults."), *GaitTag.ToString(), *GetName());
	
	return FVMMovementGaitSettings();
}