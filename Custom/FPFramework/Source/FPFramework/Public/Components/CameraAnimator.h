// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/ViewmodelController.h"
#include "Data/CameraShakeData.h"
#include "CameraAnimator.generated.h"

// ---------------------------------------------------------------
//  FActiveShake  — internal per-shake runtime state (not exposed)
// ---------------------------------------------------------------

struct FActiveShake
{
    UCameraShakeData* Data = nullptr;
    float Playback = 0.0f;
    FVector RandomizedTarget = FVector::ZeroVector;
    FVector Current = FVector::ZeroVector;
};

// ---------------------------------------------------------------
//  UCameraAnimator
//
//  Procedural camera shake component. Reads CameraBobRotation from
//  ViewmodelController and blends it with active shake impulses.
//
//  Supports up to MaxShakeSlots simultaneous shakes — oldest slot
//  is evicted when the queue is full.
//
//  Output: GetCameraOutput() — FRotator to be applied to the camera.
// ---------------------------------------------------------------

UCLASS(ClassGroup = (WonderveilStudios), meta = (BlueprintSpawnableComponent))
class FPFRAMEWORK_API UCameraAnimator : public UActorComponent
{
    GENERATED_BODY()

public:
    UCameraAnimator();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Camera|Shake")
    void PlayCameraShake(UCameraShakeData* ShakeData);

    // Final blended camera rotation this frame (shake + bob bleed)
    UFUNCTION(BlueprintPure, Category = "Camera")
    FRotator GetCameraOutput() const { return CameraOutput; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Shake", meta = (UIMin = "1", UIMax = "8"))
    int32 MaxShakeSlots = 3;

private:
    void UpdateShakes(float DeltaTime);

    UPROPERTY()
    TObjectPtr<UViewmodelController> ViewmodelController;

    FRotator BaseCameraRotation = FRotator::ZeroRotator;
    
    // Active shake queue
    TArray<FActiveShake> ActiveShakes;
    
    FRotator CameraOutput = FRotator::ZeroRotator;
};