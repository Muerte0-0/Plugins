// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#include "Data/IKMotionData.h"
#include "Kismet/KismetMathLibrary.h"

FTransform UIKMotionData::Evaluate(float Time) const
{
	FTransform Result = FTransform::Identity;

	if (PositionCurve)
	{
		const FVector Pos = PositionCurve->GetVectorValue(Time) * PositionScale;
		Result.SetLocation(Pos);
	}

	if (RotationCurve)
	{
		const FVector RotVec = RotationCurve->GetVectorValue(Time) * RotationScale;
		// Curve channels: X = Pitch, Y = Yaw, Z = Roll
		Result.SetRotation(FQuat(FRotator(RotVec.X, RotVec.Y, RotVec.Z)));
	}

	return Result;
}

float UIKMotionData::GetLength() const
{
	float PosMin = 0.0f, PosMax = 0.0f;
	float RotMin = 0.0f, RotMax = 0.0f;

	if (PositionCurve) PositionCurve->GetTimeRange(PosMin, PosMax);
	if (RotationCurve) RotationCurve->GetTimeRange(RotMin, RotMax);

	return UKismetMathLibrary::FMax(PosMax, RotMax);
}