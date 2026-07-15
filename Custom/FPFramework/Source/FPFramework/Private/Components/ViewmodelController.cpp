// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#include "Components/ViewmodelController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UViewmodelController::UViewmodelController()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UViewmodelController::BeginPlay()
{
    Super::BeginPlay();

    ActiveViewmodelData = DefaultViewmodelData;
    ContextValid = UKismetSystemLibrary::DoesImplementInterface(GetOwner(), UViewmodelContext::StaticClass());
    
    if (ContextValid)
    {
        PreviousViewRotation = IViewmodelContext::Execute_GetViewRotation(GetOwner());
        
        if (ActiveViewmodelData)
        {
            const FGameplayTag InitialGait = IViewmodelContext::Execute_GetCurrentGait(GetOwner());
            
            MovementState.LastGaitTag = InitialGait;
            MovementState.ActiveGaitCfg = ActiveViewmodelData->GetGaitSettings(InitialGait);
            MovementState.TargetGaitCfg = MovementState.ActiveGaitCfg;
            
            if (ActiveViewmodelData->bEnableLeftHand)
                MovementState.LeftHandPhaseTime = ActiveViewmodelData->LeftHandPhaseOffset;
        }
    }
}

void UViewmodelController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!ActiveViewmodelData) return;
    if (!ContextValid) return;

    MovementState.NoiseTime = FMath::Fmod(MovementState.NoiseTime + DeltaTime, 1000.0f);

    UpdateSway(DeltaTime);
    UpdateMovement(DeltaTime);
    UpdateLag(DeltaTime);
    UpdateIKMotion(DeltaTime);
}

void UViewmodelController::SetViewmodelData(UViewmodelData* NewData)
{
    if (NewData)
    {
        ActiveViewmodelData = NewData;
        
        const FGameplayTag CurrentGait = IViewmodelContext::Execute_GetCurrentGait(GetOwner());
        MovementState.LastGaitTag   = CurrentGait;
        MovementState.ActiveGaitCfg = ActiveViewmodelData->GetGaitSettings(CurrentGait);
        MovementState.TargetGaitCfg = MovementState.ActiveGaitCfg;

        if (ActiveViewmodelData->bEnableLeftHand)
            MovementState.LeftHandPhaseTime = ActiveViewmodelData->LeftHandPhaseOffset;
    }
    else
        ResetViewmodelData();
}

void UViewmodelController::ResetViewmodelData()
{
    ActiveViewmodelData = DefaultViewmodelData;
    
    const FGameplayTag CurrentGait = IViewmodelContext::Execute_GetCurrentGait(GetOwner());
    MovementState.LastGaitTag   = CurrentGait;
    MovementState.ActiveGaitCfg = ActiveViewmodelData->GetGaitSettings(CurrentGait);
    MovementState.TargetGaitCfg = MovementState.ActiveGaitCfg;

    if (ActiveViewmodelData->bEnableLeftHand)
        MovementState.LeftHandPhaseTime = ActiveViewmodelData->LeftHandPhaseOffset;
}

// ---------------------------------------------------------------
//  Movement
// ---------------------------------------------------------------

void UViewmodelController::UpdateMovement(float DeltaTime)
{
    const FGameplayTag Gait = IViewmodelContext::Execute_GetCurrentGait(GetOwner());
    const FVector Velocity = IViewmodelContext::Execute_GetCurrentVelocity(GetOwner());
    const bool bFalling = IViewmodelContext::Execute_GetIsFalling(GetOwner());
    
    if (Gait != MovementState.LastGaitTag)
    {
        MovementState.LastGaitTag  = Gait;
        MovementState.TargetGaitCfg = ActiveViewmodelData->GetGaitSettings(Gait);
    }

    FVMMovementGaitSettings& activeGait = MovementState.ActiveGaitCfg;
    const FVMMovementGaitSettings& targetGait = MovementState.TargetGaitCfg;
    const float gaitInterpSpeed = ActiveViewmodelData->GaitTransitionSpeed;

    activeGait.PhaseSpeed = FMath::FInterpTo(activeGait.PhaseSpeed, targetGait.PhaseSpeed, DeltaTime, gaitInterpSpeed);
    activeGait.FullAlphaSpeed = FMath::FInterpTo(activeGait.FullAlphaSpeed, targetGait.FullAlphaSpeed, DeltaTime, gaitInterpSpeed);
    activeGait.PositionScale.X = FMath::FInterpTo(activeGait.PositionScale.X, targetGait.PositionScale.X, DeltaTime, gaitInterpSpeed);
    activeGait.PositionScale.Y = FMath::FInterpTo(activeGait.PositionScale.Y, targetGait.PositionScale.Y, DeltaTime, gaitInterpSpeed);
    activeGait.PositionScale.Z = FMath::FInterpTo(activeGait.PositionScale.Z, targetGait.PositionScale.Z, DeltaTime, gaitInterpSpeed);
    activeGait.RotationScale.Pitch = FMath::FInterpTo(activeGait.RotationScale.Pitch, targetGait.RotationScale.Pitch, DeltaTime, gaitInterpSpeed);
    activeGait.RotationScale.Yaw = FMath::FInterpTo(activeGait.RotationScale.Yaw, targetGait.RotationScale.Yaw, DeltaTime, gaitInterpSpeed);
    activeGait.RotationScale.Roll = FMath::FInterpTo(activeGait.RotationScale.Roll, targetGait.RotationScale.Roll, DeltaTime, gaitInterpSpeed);

    const float Speed = Velocity.Size() * 0.86f;
    const float RawAlpha = (activeGait.FullAlphaSpeed > KINDA_SMALL_NUMBER) ? FMath::Clamp(Speed / activeGait.FullAlphaSpeed, 0.0f, 1.0f) : 0.0f;
    const float DeadAlpha = (RawAlpha < ActiveViewmodelData->AlphaDeadzone) ? 0.0f : RawAlpha;
    const float TargetAlpha = bFalling ? DeadAlpha * ActiveViewmodelData->FallingAlphaScale : DeadAlpha;

    MovementState.Alpha = FMath::FInterpTo(MovementState.Alpha, TargetAlpha, DeltaTime, ActiveViewmodelData->AlphaInterpSpeed);
    AnimState.MoveAnimAlpha = MovementState.Alpha;

    if (MovementState.Alpha < KINDA_SMALL_NUMBER)
    {
        AnimState.MoveAnimPos = FVector::ZeroVector;
        AnimState.MoveAnimRot = FRotator::ZeroRotator;
        AnimState.CameraMovementRotation = FRotator::ZeroRotator;
        AnimState.LeftHandMovePos = FVector::ZeroVector;
        AnimState.LeftHandMoveRot = FRotator::ZeroRotator;
        return;
    }

    MovementState.PhaseTime += DeltaTime * activeGait.PhaseSpeed * MovementState.Alpha;
    if (MovementState.PhaseTime > 1.0f) MovementState.PhaseTime -= 1.0f;

    const float PhaseNoise = FMath::PerlinNoise1D(MovementState.NoiseTime * 0.5f) * ActiveViewmodelData->NoisePhaseScale;
    float NoisyPhase = MovementState.PhaseTime + PhaseNoise;
    if (NoisyPhase > 1.0f) NoisyPhase -= 1.0f;
    if (NoisyPhase < 0.0f) NoisyPhase += 1.0f;

    const float AmpNoise = FMath::PerlinNoise1D(MovementState.NoiseTime * 0.3f);
    const float AmpMult = FMath::Lerp(1.0f - ActiveViewmodelData->NoiseAmplitudeRange, 1.0f + ActiveViewmodelData->NoiseAmplitudeRange, (AmpNoise + 1.0f) * 0.5f);

    FVector Pos;
    Pos.X = ActiveViewmodelData->SampleMovementPosX(NoisyPhase) * activeGait.PositionScale.X;
    Pos.Y = ActiveViewmodelData->SampleMovementPosY(NoisyPhase) * activeGait.PositionScale.Y;
    Pos.Z = ActiveViewmodelData->SampleMovementPosZ(NoisyPhase) * activeGait.PositionScale.Z;
    Pos *= AmpMult * MovementState.Alpha;

    const float RotNoise = FMath::PerlinNoise1D(MovementState.NoiseTime * 0.4f);

    FRotator Rot;
    Rot.Pitch = ActiveViewmodelData->SampleMovementPitch(NoisyPhase) * activeGait.RotationScale.Pitch;
    Rot.Yaw = ActiveViewmodelData->SampleMovementYaw(NoisyPhase)   * activeGait.RotationScale.Yaw  + RotNoise * ActiveViewmodelData->NoiseRotYawScale;
    Rot.Roll = ActiveViewmodelData->SampleMovementRoll(NoisyPhase)  * activeGait.RotationScale.Roll + RotNoise * ActiveViewmodelData->NoiseRotRollScale;
    Rot *= AmpMult * MovementState.Alpha;

    AnimState.MoveAnimPos = Pos;
    AnimState.MoveAnimRot = Rot;
    
    AnimState.CameraMovementRotation = Rot * ActiveViewmodelData->CameraMovementScale;
    
    if (ActiveViewmodelData->bEnableLeftHand)
    {
        MovementState.LeftHandPhaseTime += DeltaTime * activeGait.PhaseSpeed * MovementState.Alpha;
        if (MovementState.LeftHandPhaseTime > 1.0f) MovementState.LeftHandPhaseTime -= 1.0f;

        float LHPhase = MovementState.LeftHandPhaseTime + PhaseNoise;
        if (LHPhase > 1.0f) LHPhase -= 1.0f;
        if (LHPhase < 0.0f) LHPhase += 1.0f;

        FVector LHPos;
        LHPos.X = ActiveViewmodelData->SampleMovementPosX(LHPhase) * activeGait.PositionScale.X;
        LHPos.Y = ActiveViewmodelData->SampleMovementPosY(LHPhase) * activeGait.PositionScale.Y;
        LHPos.Z = ActiveViewmodelData->SampleMovementPosZ(LHPhase) * activeGait.PositionScale.Z;
        LHPos *= AmpMult * MovementState.Alpha;

        FRotator LHRot;
        LHRot.Pitch = ActiveViewmodelData->SampleMovementPitch(LHPhase) * activeGait.RotationScale.Pitch;
        LHRot.Yaw = ActiveViewmodelData->SampleMovementYaw(LHPhase) * activeGait.RotationScale.Yaw + RotNoise * ActiveViewmodelData->NoiseRotYawScale;
        LHRot.Roll = ActiveViewmodelData->SampleMovementRoll(LHPhase) * activeGait.RotationScale.Roll + RotNoise * ActiveViewmodelData->NoiseRotRollScale;
        LHRot *= AmpMult * MovementState.Alpha;

        AnimState.LeftHandMovePos = LHPos;
        AnimState.LeftHandMoveRot = LHRot;
    }
    else
    {
        AnimState.LeftHandMovePos = AnimState.MoveAnimPos;
        AnimState.LeftHandMoveRot = AnimState.MoveAnimRot;
    }
}

// ---------------------------------------------------------------
//  Lag
// ---------------------------------------------------------------

void UViewmodelController::UpdateLag(float DeltaTime)
{
    const FVector Velocity = IViewmodelContext::Execute_GetCurrentVelocity(GetOwner());
    const float MaxSpeed = IViewmodelContext::Execute_GetMaxMoveSpeed(GetOwner());
    
    const AActor* Owner = GetOwner();

    const float RightCh = FVector::DotProduct(Velocity, Owner->GetActorRightVector()) /  MaxSpeed;
    const float FwdCh = FVector::DotProduct(Velocity, Owner->GetActorForwardVector()) / -MaxSpeed;
    const float UpCh = FVector::DotProduct(Velocity, Owner->GetActorUpVector()) / -ActiveViewmodelData->LagJumpSpeed;

    const FVector LagTarget = UKismetMathLibrary::ClampVectorSize(FVector(RightCh, FwdCh, UpCh) * ActiveViewmodelData->LagVelocityScale, 0.0f, ActiveViewmodelData->LagMaxMagnitude);

    AnimState.LocationLagPos = UKismetMathLibrary::VInterpTo(AnimState.LocationLagPos, LagTarget, DeltaTime, ActiveViewmodelData->LagInterpSpeed);

    const FRotator TiltTarget  = FRotator(0.0f, 0.0f, AnimState.LocationLagPos.Z * -2.0f);
    AnimState.InAirTilt = UKismetMathLibrary::RInterpTo(AnimState.InAirTilt, TiltTarget, DeltaTime, ActiveViewmodelData->InAirInterpSpeed);

    const FVector OffsetTarget = FVector(0.0f, 0.0f, AnimState.LocationLagPos.Z * -0.5f);
    AnimState.InAirOffset = UKismetMathLibrary::VInterpTo(AnimState.InAirOffset, OffsetTarget, DeltaTime, ActiveViewmodelData->InAirInterpSpeed);
}

// ---------------------------------------------------------------
//  Sway
// ---------------------------------------------------------------

void UViewmodelController::UpdateSway(float DeltaTime)
{
    const FRotator CurrentView = IViewmodelContext::Execute_GetViewRotation(GetOwner());
    FRotator CamDelta = UKismetMathLibrary::NormalizedDeltaRotator(CurrentView, PreviousViewRotation);
    PreviousViewRotation = CurrentView;

    CamDelta *= ActiveViewmodelData->SwayInputScale;

    FRotator SwayTarget;
    SwayTarget.Roll = FMath::Clamp( CamDelta.Pitch, -ActiveViewmodelData->SwayMaxAngle.Y, ActiveViewmodelData->SwayMaxAngle.Y);
    SwayTarget.Yaw = FMath::Clamp(-CamDelta.Yaw, -ActiveViewmodelData->SwayMaxAngle.X, ActiveViewmodelData->SwayMaxAngle.X);
    SwayTarget.Pitch = 0.0f;

    const FRotator SpringForce = (SwayTarget - SwayState.CurrentOffset) * ActiveViewmodelData->SwayStiffness;

    SwayState.SpringVelocity += SpringForce * DeltaTime;
    SwayState.SpringVelocity = SwayState.SpringVelocity * FMath::Exp(-ActiveViewmodelData->SwayDamping * DeltaTime);
    SwayState.CurrentOffset += SwayState.SpringVelocity * DeltaTime;

    AnimState.SwayRotation = SwayState.CurrentOffset;
}

// ---------------------------------------------------------------
//  IK Motion Playback
// ---------------------------------------------------------------

void UViewmodelController::PlayIKMotion(UIKMotionData* Motion)
{
    if (!Motion) return;

    IKState.ActiveMotion = Motion;
    IKState.CachedMotion = AnimState.IKMotion;
    IKState.Playback = 0.0f;
    IKState.bPlaying = true;
}

void UViewmodelController::UpdateIKMotion(float DeltaTime)
{
    if (!IKState.bPlaying || !IKState.ActiveMotion) return;

    IKState.Playback += IKState.ActiveMotion->PlayRate * DeltaTime;

    const FTransform Evaluated = IKState.ActiveMotion->Evaluate(IKState.Playback);
    const float Length = IKState.ActiveMotion->GetLength();

    // Blend-in alpha
    float Alpha = 1.0f;
    if (!FMath::IsNearlyZero(IKState.ActiveMotion->BlendTime))
        Alpha = FMath::Clamp(IKState.Playback / IKState.ActiveMotion->BlendTime, 0.0f, 1.0f);

    // Blend-out alpha
    if (!FMath::IsNearlyZero(IKState.ActiveMotion->BlendOutTime))
    {
        const float BlendOutStart = Length - IKState.ActiveMotion->BlendOutTime;
        if (IKState.Playback >= BlendOutStart)
        {
            const float OutAlpha = FMath::Clamp((IKState.Playback - BlendOutStart) / IKState.ActiveMotion->BlendOutTime, 0.0f, 1.0f);
            Alpha = FMath::Min(Alpha, 1.0f - OutAlpha);
        }
    }

    AnimState.IKMotion = UKismetMathLibrary::TLerp(IKState.CachedMotion, Evaluated, Alpha);

    if (IKState.Playback >= Length)
    {
        IKState.bPlaying   = false;
        AnimState.IKMotion = FTransform::Identity;
    }
}