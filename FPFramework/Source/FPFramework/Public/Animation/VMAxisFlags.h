// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Misc/EnumClassFlags.h"
#include "VMAxisFlags.generated.h"

UENUM(BlueprintType, Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EVMAxisFlags : uint8
{
	None = 0      UMETA(Hidden),
	X    = 1 << 0 UMETA(DisplayName = "X (Roll)"),
	Y    = 1 << 1 UMETA(DisplayName = "Y (Pitch)"),
	Z    = 1 << 2 UMETA(DisplayName = "Z (Yaw)"),
};
ENUM_CLASS_FLAGS(EVMAxisFlags)
