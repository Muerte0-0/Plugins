// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Camera/CameraComponent.h"
#include "UObject/Interface.h"
#include "IViewmodelContext.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UViewmodelContext : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FPFRAMEWORK_API IViewmodelContext
{
	GENERATED_BODY()

public:
	// Current gait tag — "Movement.Gait.Walk" etc. EmptyTag when idle/airborne
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ViewmodelContext")
	FGameplayTag GetCurrentGait() const;

	// World-space Velocity
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ViewmodelContext")
	FVector GetCurrentVelocity() const;

	// Max horizontal speed at current gait — normalises velocity to bob alpha
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ViewmodelContext")
	float GetMaxMoveSpeed() const;

	// True while airborne
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ViewmodelContext")
	bool GetIsFalling() const;

	// View/aim direction. Player → ControlRotation. AI → Virtual Camera Rotator.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ViewmodelContext")
	FRotator GetViewRotation() const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ViewmodelContext")
	UCameraComponent* GetActiveCamera() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ViewmodelContext")
	USkeletalMeshComponent* GetViewmodelMesh() const;
};
