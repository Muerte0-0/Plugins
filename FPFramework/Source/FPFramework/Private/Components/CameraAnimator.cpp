// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#include "Components/CameraAnimator.h"

#include "Camera/CameraComponent.h"
#include "Components/ViewmodelController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UCameraAnimator::UCameraAnimator()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UCameraAnimator::BeginPlay()
{
    Super::BeginPlay();

    ViewmodelController = GetOwner()->GetComponentByClass<UViewmodelController>();
    
    if (UKismetSystemLibrary::DoesImplementInterface(GetOwner(), UViewmodelContext::StaticClass()))
        if (UCameraComponent* ActiveCamera = IViewmodelContext::Execute_GetActiveCamera(GetOwner()))
            BaseCameraRotation = ActiveCamera->GetRelativeRotation();
}

void UCameraAnimator::PlayCameraShake(UCameraShakeData* ShakeData)
{
    if (!ShakeData || !ShakeData->RotationCurve) return;
    
    if (!UKismetSystemLibrary::DoesImplementInterface(GetOwner(), UViewmodelContext::StaticClass())) return;

    // Evict oldest slot if full
    if (ActiveShakes.Num() >= MaxShakeSlots) ActiveShakes.RemoveAt(0);

    FActiveShake NewShake;
    NewShake.Data = ShakeData;
    NewShake.Playback = 0.0f;
    NewShake.Current = FVector::ZeroVector;
    NewShake.RandomizedTarget = FVector(
        FMath::RandRange(ShakeData->Roll.X,  ShakeData->Roll.Y),    // X = Roll
        FMath::RandRange(ShakeData->Pitch.X, ShakeData->Pitch.Y),   // Y = Pitch
        FMath::RandRange(ShakeData->Yaw.X,   ShakeData->Yaw.Y)      // Z = Yaw
    );

    ActiveShakes.Add(NewShake);
}

void UCameraAnimator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateShakes(DeltaTime);
}

void UCameraAnimator::UpdateShakes(float DeltaTime)
{
    // Start from camera bob bleed if Viewmodel Controller is present
    FVector Accumulated = FVector::ZeroVector;

    if (ViewmodelController)
    {
        const FRotator BobRot = ViewmodelController->GetAnimState().CameraMovementRotation;
        // CameraBobRotation: Pitch → Y, Yaw → Z, Roll → X (match shake convention)
        Accumulated += FVector(BobRot.Roll, BobRot.Pitch, BobRot.Yaw);
    }

    // Tick all active shakes and accumulate their outputs
    for (int32 i = ActiveShakes.Num() - 1; i >= 0; --i)
    {
        FActiveShake& Shake = ActiveShakes[i];

        if (!Shake.Data || !Shake.Data->RotationCurve)
        {
            ActiveShakes.RemoveAt(i);
            continue;
        }

        Shake.Playback += DeltaTime * Shake.Data->PlayRate;

        const FVector CurveVal = Shake.Data->RotationCurve->GetVectorValue(Shake.Playback) * Shake.RandomizedTarget;

        // Smooth toward curve target
        Shake.Current = UKismetMathLibrary::VInterpTo(Shake.Current, CurveVal, DeltaTime, Shake.Data->Smoothing);

        Accumulated += Shake.Current;

        // Check if past end of curve
        float TimeMin, TimeMax;
        Shake.Data->RotationCurve->GetTimeRange(TimeMin, TimeMax);

        if (Shake.Playback >= TimeMax)
            ActiveShakes.RemoveAt(i);
    }

    // Accumulated: X=Roll, Y=Pitch, Z=Yaw → FRotator(Pitch, Yaw, Roll)
    CameraOutput = FRotator(Accumulated.Y, Accumulated.Z, Accumulated.X);
    
    if (UCameraComponent* ActiveCamera = IViewmodelContext::Execute_GetActiveCamera(GetOwner()))
        ActiveCamera->SetRelativeRotation(BaseCameraRotation + CameraOutput);
}