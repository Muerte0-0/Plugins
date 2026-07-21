// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/ViewmodelData.h"
#include "Data/IKMotionData.h"
#include "Interfaces/IViewmodelContext.h"
#include "ViewmodelController.generated.h"

// ---------------------------------------------------------------
//  Footstep notify
// ---------------------------------------------------------------

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFootstep, bool, bIsLeftFoot);

// ---------------------------------------------------------------
//  Anim State
// ---------------------------------------------------------------

USTRUCT(BlueprintType)
struct FPFRAMEWORK_API FVMAnimState
{
    GENERATED_BODY()

    // Movement
    UPROPERTY(BlueprintReadOnly, Category = "Viewmodel|Movement")
    FVector MoveAnimPos = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category = "Viewmodel|Movement")
    FRotator MoveAnimRot = FRotator::ZeroRotator;
    
    UPROPERTY(BlueprintReadOnly, Category = "Viewmodel|Movement")
    FVector LeftHandMovePos = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category = "Viewmodel|Movement")
    FRotator LeftHandMoveRot = FRotator::ZeroRotator;

    UPROPERTY(BlueprintReadOnly, Category = "Viewmodel|Movement")
    float MoveAnimAlpha = 0.0f;

    // Lag
    UPROPERTY(BlueprintReadOnly, Category = "Viewmodel|Lag")
    FVector LocationLagPos = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category = "Viewmodel|Lag")
    FRotator InAirTilt = FRotator::ZeroRotator;

    UPROPERTY(BlueprintReadOnly, Category = "Viewmodel|Lag")
    FVector InAirOffset = FVector::ZeroVector;

    // Sway
    UPROPERTY(BlueprintReadOnly, Category = "Viewmodel|Sway")
    FRotator SwayRotation = FRotator::ZeroRotator;

    // IK Motion (equip, ads, inspect, landing)
    UPROPERTY(BlueprintReadOnly, Category = "Viewmodel|IK")
    FTransform IKMotion = FTransform::Identity;

    // Camera Movement bleed — fraction of MoveAnimRot fed to CameraAnimator
    UPROPERTY(BlueprintReadOnly, Category = "Viewmodel|Camera")
    FRotator CameraMovementRotation = FRotator::ZeroRotator;
};

// ---------------------------------------------------------------
//  Internal spring state — not Blueprint exposed
// ---------------------------------------------------------------

struct FVMSwayState
{
    FRotator CurrentOffset  = FRotator::ZeroRotator;
    FRotator SpringVelocity = FRotator::ZeroRotator;
};

struct FVMMovementState
{
    float PhaseTime = 0.0f;
    float LeftHandPhaseTime = 0.0f;
    float NoiseTime = 0.0f;
    float Alpha = 0.0f;
    float AlphaVelocity = 0.0f;

    float PreviousPhaseTime = 0.0f;
    
    FGameplayTag LastGaitTag;
    FVMMovementGaitSettings ActiveGaitCfg;
    FVMMovementGaitSettings TargetGaitCfg;
};

// ---------------------------------------------------------------
//  IK playback state — not Blueprint exposed
// ---------------------------------------------------------------

struct FVMIKState
{
    TObjectPtr<UIKMotionData> ActiveMotion = nullptr;
    FTransform CachedMotion = FTransform::Identity;
    float Playback = 0.0f;
    bool bPlaying = false;
};

// ---------------------------------------------------------------
//  Viewmodel Controller
// ---------------------------------------------------------------

UCLASS(ClassGroup = (WonderveilStudios), meta = (BlueprintSpawnableComponent))
class FPFRAMEWORK_API UViewmodelController : public UActorComponent
{
    GENERATED_BODY()

public:
    UViewmodelController();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Viewmodel")
    TObjectPtr<UViewmodelData> DefaultViewmodelData;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Viewmodel")
	UViewmodelData* GetActiveViewmodelData() { return ActiveViewmodelData; }
	
    UFUNCTION(BlueprintCallable, Category = "Viewmodel")
    void SetViewmodelData(UViewmodelData* NewData);

    UFUNCTION(BlueprintCallable, Category = "Viewmodel")
    void ResetViewmodelData();

    // Trigger an IK motion (equip, ads-in, inspect, etc.)
    UFUNCTION(BlueprintCallable, Category = "Viewmodel|IK")
    void PlayIKMotion(UIKMotionData* Motion);

    UFUNCTION(BlueprintPure, Category = "Viewmodel")
    const FVMAnimState& GetAnimState() const { return AnimState; }

    // Fired when the viewmodel bob cycle crosses a configured footstep phase.
    // LOCAL ONLY — reflects the owning client's arms viewmodel, not authoritative movement.
    UPROPERTY(BlueprintAssignable, Category = "Viewmodel|Footsteps")
    FOnFootstep OnFootstep;

private:
    void UpdateMovement(float DeltaTime);
    void UpdateLag(float DeltaTime);
    void UpdateSway(float DeltaTime);
    void UpdateIKMotion(float DeltaTime);
    void UpdateFootsteps();

    UPROPERTY()
    TObjectPtr<UViewmodelData> ActiveViewmodelData;

    FVMSwayState SwayState;
    FVMMovementState MovementState;
    FVMIKState IKState;

    FRotator PreviousViewRotation = FRotator::ZeroRotator;
    
    bool ContextValid;

    FVMAnimState AnimState;
};