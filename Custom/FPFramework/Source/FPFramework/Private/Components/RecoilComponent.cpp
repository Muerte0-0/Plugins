// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#include "Components/RecoilComponent.h"

#include "Curves/CurveVector.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogRecoil);

// ---------------------------------------------------------------
//  Constructor / lifecycle
// ---------------------------------------------------------------

URecoilComponent::URecoilComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void URecoilComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupComponent();
	SetupStateMachine();
	SetComponentTickEnabled(false);
}

void URecoilComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(RecoilData))
	{
		return;
	}

	TickControllerRecoil(DeltaTime);
	TickAdvancedSolver(DeltaTime);

	// Once both the curve playback and the interp tail have settled, disable tick
	// until the next Play() call.
	if (IsComponentTickEnabled() && !bCurvePlaying)
	{
		const bool bAnimationStopped = LerpedRot.Equals(OutRot, 0.001f) && LerpedLoc.Equals(OutLoc, 0.001f);
		const bool bControllerStopped = Recoil.IsNearlyZero();

		if (bAnimationStopped && bControllerStopped)
		{
			SetComponentTickEnabled(false);
		}
	}
}

void URecoilComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(URecoilComponent, OutRot, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(URecoilComponent, OutLoc, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(URecoilComponent, InputScale, COND_SkipOwner);
}

// ---------------------------------------------------------------
//  Setup
// ---------------------------------------------------------------

void URecoilComponent::SetupComponent()
{
	SetIsReplicated(true);
	InputScale.Reset();
	OwnerCharacter = Cast<APawn>(GetOwner());
}

void URecoilComponent::SetupStateMachine()
{
	FRecoilAnimState SemiState;
	FRecoilAnimState AutoState;

	// -----------------------------------------------------------
	//  Semi state
	//  Selected on the first shot, or whenever the inter-shot gap
	//  is larger than one fire-rate interval (i.e. the player paused
	//  before pulling the trigger again).
	// -----------------------------------------------------------

	SemiState.TransitionCondition.BindLambda([this]() -> bool
	{
		// One full fire-rate interval plus one frame of tolerance
		const float TimerError = (60.f / FireRate) / GetWorld()->GetDeltaSeconds() + 1;
		const float Threshold = TimerError * GetWorld()->GetDeltaSeconds();

		if (bEnableSmoothing)
		{
			bEnableSmoothing = false;
		}

		return FireMode == EFireMode::Semi
		    || (GetDelta() > Threshold + 0.01f && !bCurveLooping);
	});

	SemiState.OnPlay.BindLambda([this]() -> void
	{
		SetupTransition(LerpedRot, LerpedLoc, RecoilData->SingleRot, RecoilData->SingleLoc);
	});

	SemiState.OnStop.BindLambda([this]() -> void
	{
		// Nothing to do for semi — looping was never active.
	});

	// -----------------------------------------------------------
	//  Auto state
	//  Selected on every subsequent shot while the trigger is held.
	//  Locks the curve into a looping mode pinned to the fire rate.
	// -----------------------------------------------------------

	AutoState.TransitionCondition.BindLambda([this]() -> bool
	{
		return true; // fallthrough — always valid if semi didn't match
	});

	AutoState.OnPlay.BindLambda([this]() -> void
	{
		bEnableSmoothing = true;

		if (bCurveLooping)
		{
			// Already looping — subsequent shots inside full-auto just
			// recalculate targets; no transition needed.
			return;
		}

		// Compute the blend start value at the corrected playback position
		// so the transition from semi to auto is seamless.
		const float Correction = 60.f / FireRate + RecoilData->PlaybackOffset;

		UCurveVector* RotCurve = RecoilData->AutoRot ? RecoilData->AutoRot : RecoilData->SingleRot;
		UCurveVector* LocCurve = RecoilData->AutoLoc ? RecoilData->AutoLoc : RecoilData->SingleLoc;

		CorrectAlpha(RotCurve, LocCurve, Correction);
		SetupTransition(StartValRot, StartValLoc, RotCurve, LocCurve);

		// Pin the curve length to the fire interval and begin looping
		CurveLength   = 60.f / FireRate;
		bCurveLooping = true;

		PushbackTarget = RecoilData->Pushback;
	});

	AutoState.OnStop.BindLambda([this]() -> void
	{
		if (!bAnimActive)
		{
			// OnStop already fired — not a valid state.
			return;
		}

		if (bEnableSmoothing)
		{
			// Release the loop and let the curve play to its natural end.
			// CurveLength will be re-derived from the keyframe on the
			// next SetupTransition call.
			bCurveLooping = false;
		}
	});

	StateMachine.Add(SemiState);
	StateMachine.Add(AutoState);
}

// ---------------------------------------------------------------
//  Public API
// ---------------------------------------------------------------

void URecoilComponent::Init(URecoilData* NewRecoilData, const float Rate, const int32 Bursts)
{
	if (!IsValid(NewRecoilData))
	{
		UE_LOG(LogRecoil, Warning, TEXT("Init: RecoilData is null — aborting."));
		return;
	}

	RecoilData = NewRecoilData;

	Compensation = TargetRecoil = Recoil = FVector2D::ZeroVector;

	BurstAmount  = BurstCounter = Bursts;
	FireRate = FMath::Max(Rate, 0.001f);

	if (!(RecoilData->SingleRot && RecoilData->SingleLoc))
	{
		UE_LOG(LogRecoil, Warning, TEXT("Init: SingleRot or SingleLoc curve is null — output will be identity."));
		return;
	}

	// Reset targets
	TargetRot = FRotator::ZeroRotator;
	TargetLoc = FVector::ZeroVector;
	Noise2DTarget = FVector2D::ZeroVector;
	PitchProgress.Y = ZProgress.Y = PushbackTarget = 0.0;

	// Preserve previously seeded speeds if they are already non-zero
	NoiseSpeed.X = GetNonZero(NoiseSpeed.X,       RecoilData->Noise2DAccel.X);
	NoiseSpeed.Y = GetNonZero(NoiseSpeed.Y,       RecoilData->Noise2DAccel.Y);
	PitchProgressSpeed = GetNonZero(PitchProgressSpeed,  RecoilData->PitchProgress.Accel);
	ZProgressSpeed = GetNonZero(ZProgressSpeed,      RecoilData->ZProgress.Accel);
	PushbackSpeed = GetNonZero(PushbackSpeed,       RecoilData->PushbackAccel);

	// Seed the playrate from the data asset
	CurvePlayRate = RecoilData->PlayRate;
}

void URecoilComponent::Play()
{
	if (!IsValid(RecoilData)) return;

	if (!IsComponentTickEnabled()) SetComponentTickEnabled(true);

	SetControllerRecoilTarget();

	CurvePlayRate = RecoilData->PlayRate;
	bAnimActive = true;

	// Select the appropriate state (semi or auto)
	for (uint16 i = 0; i < StateMachine.Num(); i++)
	{
		if (StateMachine[i].TransitionCondition.Execute())
		{
			CurrentStateIndex = i;
			break;
		}
	}

	if (!StateMachine[CurrentStateIndex].OnPlay.ExecuteIfBound())
		UE_LOG(LogRecoil, Warning, TEXT("Play: state %i OnPlay is not bound."), CurrentStateIndex);

	// Fire target randomisation immediately — this replaces the FTimeline
	// event that was previously bound at time 0.
	CalculateTargetData();

	LastShotTime = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
}

void URecoilComponent::Stop()
{
	bCurveLooping = false;

	if (!StateMachine[CurrentStateIndex].OnStop.ExecuteIfBound())
		UE_LOG(LogRecoil, Warning, TEXT("Stop: state %i OnStop is not bound."), CurrentStateIndex);

	bAnimActive = false;

	RefreshControllerCompensation();
}

void URecoilComponent::SetFireMode(const EFireMode NewMode)
{
	FireMode = NewMode;
}

void URecoilComponent::SetAimingStatus(const bool bStatus)
{
	bIsAiming = bStatus;
}

void URecoilComponent::ScaleInput(FRecoilInputScale NewScale)
{
	InputScale *= NewScale;
}

void URecoilComponent::UnscaleInput(FRecoilInputScale NewScale)
{
	InputScale /= NewScale;
}

void URecoilComponent::ResetInputScale()
{
	InputScale.Reset();
}

float URecoilComponent::GetDelta() const
{
	return UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld()) - LastShotTime;
}

// ---------------------------------------------------------------
//  Tick — advanced solver
// ---------------------------------------------------------------

void URecoilComponent::TickAdvancedSolver(float DeltaTime)
{
	if (!(TempRot && TempLoc))
		return;

	// ---------------------------------------------------------
	//  Advance the playhead (replaces FTimeline::TickTimeline)
	// ---------------------------------------------------------
	if (bCurvePlaying)
	{
		CurveTime += DeltaTime * CurvePlayRate;

		if (CurveTime >= CurveLength)
		{
			if (bCurveLooping)
			{
				// Wrap playhead and fire target randomisation each loop
				CurveTime = FMath::Fmod(CurveTime, CurveLength);
				CalculateTargetData();
			}
			else
			{
				CurveTime     = CurveLength;
				bCurvePlaying = false;
			}
		}
	}

	// ---------------------------------------------------------
	//  Sample curves at current playhead position
	// ---------------------------------------------------------
	AnimationUpdate();

	// ---------------------------------------------------------
	//  Build finalised transform from all additive layers
	// ---------------------------------------------------------
	FTransform Finalized = FTransform(LerpedRot, LerpedLoc);

	ApplyNoise(Finalized, DeltaTime);
	ApplyPushback(Finalized, DeltaTime);
	ApplyProgression(Finalized, DeltaTime);

	const FVector PivotOffset = bIsAiming ? RecoilData->AdsPivotOffset : RecoilData->PivotOffset;
	const FVector PivotDelta  = Finalized.Rotator().RotateVector(PivotOffset) - PivotOffset;
	Finalized.AddToTranslation(PivotDelta);

	ApplyRecoilSway(Finalized, DeltaTime);

	OutLoc = Finalized.GetLocation();
	OutRot = Finalized.Rotator();

	// Global multipliers
	OutLoc *= GlobalTranslationMultiplier;
	OutRot.Pitch *= GlobalRotationMultiplier.Y;
	OutRot.Yaw *= GlobalRotationMultiplier.Z;
	OutRot.Roll *= GlobalRotationMultiplier.X;
}

// ---------------------------------------------------------------
//  Tick — controller recoil
// ---------------------------------------------------------------

void URecoilComponent::TickControllerRecoil(float DeltaTime)
{
	if (!OwnerCharacter)
		return;

	if (IsFiring)
	{
		FVector2D PlayerInput = FVector2D::ZeroVector;
		
		if (const APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController()))
			PC->GetInputMouseDelta(PlayerInput.X, PlayerInput.Y);

		Compensation += PlayerInput;
	}

	const FVector2D CachedRecoil = Recoil;

	Recoil.X = FMath::FInterpTo(Recoil.X, TargetRecoil.X, DeltaTime, RecoilData->ControllerRecoil.RecoilSmoothing.X);
	Recoil.Y = FMath::FInterpTo(Recoil.Y, TargetRecoil.Y, DeltaTime, RecoilData->ControllerRecoil.RecoilSmoothing.Y);

	if (!IsFiring)
		TargetRecoil = FMath::Vector2DInterpTo(TargetRecoil, FVector2D::ZeroVector, DeltaTime, RecoilData->ControllerRecoil.Damping);

	OwnerCharacter->AddControllerPitchInput(Recoil.Y - CachedRecoil.Y);
	OwnerCharacter->AddControllerYawInput(Recoil.X - CachedRecoil.X);
}

// ---------------------------------------------------------------
//  AnimationUpdate — sample active curves at CurveTime
// ---------------------------------------------------------------

void URecoilComponent::AnimationUpdate()
{
	const float Playback = CurveTime;
	const float DeltaSeconds = GetWorld()->GetDeltaSeconds();
	float LastPlayback = Playback - DeltaSeconds * CurvePlayRate;
	LastPlayback = FMath::Max(LastPlayback, 0.f);

	FVector Alpha = TempRot->GetVectorValue(Playback);
	FVector LastAlpha = TempRot->GetVectorValue(LastPlayback);

	const FRotator ExtraRot = bEnableSmoothing ? RecoilData->MultiRot : FRotator(1.f, 1.f, 1.f);
	const FVector ExtraLoc = bEnableSmoothing ? RecoilData->MultiLoc : FVector(1.f, 1.f, 1.f);

	RawOutRot.Roll = !MuteRot.X ? FMath::Lerp(CorrectStart(LastAlpha.X, Alpha.X, CanRestRot.X, StartValRot.Roll), TargetRot.Roll  * ExtraRot.Roll,  Alpha.X) : 0.f;

	RawOutRot.Pitch = !MuteRot.Y ? FMath::Lerp(CorrectStart(LastAlpha.Y, Alpha.Y, CanRestRot.Y, StartValRot.Pitch), TargetRot.Pitch * ExtraRot.Pitch, Alpha.Y) : 0.f;

	RawOutRot.Yaw = !MuteRot.Z ? FMath::Lerp(CorrectStart(LastAlpha.Z, Alpha.Z, CanRestRot.Z, StartValRot.Yaw), TargetRot.Yaw   * ExtraRot.Yaw,   Alpha.Z) : 0.f;

	Alpha = TempLoc->GetVectorValue(Playback);
	LastAlpha = TempLoc->GetVectorValue(LastPlayback);

	RawOutLoc.X = !MuteLoc.X ? FMath::Lerp(CorrectStart(LastAlpha.X, Alpha.X, CanRestLoc.X, StartValLoc.X), TargetLoc.X * ExtraLoc.X, Alpha.X) : 0.f;

	RawOutLoc.Y = !MuteLoc.Y ? FMath::Lerp(CorrectStart(LastAlpha.Y, Alpha.Y, CanRestLoc.Y, StartValLoc.Y), TargetLoc.Y * ExtraLoc.Y, Alpha.Y) : 0.f;

	RawOutLoc.Z = !MuteLoc.Z ? FMath::Lerp(CorrectStart(LastAlpha.Z, Alpha.Z, CanRestLoc.Z, StartValLoc.Z), TargetLoc.Z * ExtraLoc.Z, Alpha.Z) : 0.f;

	ApplySmoothing(DeltaSeconds);
}

// ---------------------------------------------------------------
//  CalculateTargetData — randomise per-shot values
// ---------------------------------------------------------------

void URecoilComponent::CalculateTargetData()
{
	// Lambda: pick rotation targets from a profile set
	auto RandomRot = [&](FVector4 Roll, FVector2D Pitch, FVector4 Yaw)
	{
		Roll *= InputScale.Roll;
		Pitch *= InputScale.Pitch;
		Yaw *= InputScale.Yaw;

		const float Rand = GetAngularRecoil(Roll);
		const bool bSameSide = TargetRot.Roll * Rand > 0.f;

		TargetRot.Roll = RecoilData->bSmoothRoll && bSameSide ? Rand * -1.f : Rand;
		TargetRot.Pitch = FMath::FRandRange(Pitch.X, Pitch.Y);
		TargetRot.Yaw = GetAngularRecoil(Yaw);
	};

	// Lambda: pick translation targets
	auto RandomLoc = [&](FVector2D Kick, FVector2D KickR, FVector2D KickUp)
	{
		Kick *= InputScale.Kick;
		KickR *= InputScale.KickR;
		KickUp *= InputScale.KickUp;

		TargetLoc.X = FMath::FRandRange(Kick.X,   Kick.Y);
		TargetLoc.Y = FMath::FRandRange(KickR.X,  KickR.Y);
		TargetLoc.Z = FMath::FRandRange(KickUp.X, KickUp.Y);
	};

	// Lambda: accumulate 2D noise target
	auto RandomNoise = [this]()
	{
		const FVector2D Scaled = FVector2D(RecoilData->Noise2DTarget.X * InputScale.Noise.X, RecoilData->Noise2DTarget.Y * InputScale.Noise.Y);

		Noise2DTarget.X += FMath::FRandRange(-Scaled.X, Scaled.X);
		Noise2DTarget.Y += FMath::FRandRange(-Scaled.Y, Scaled.Y);
	};

	// Lambda: accumulate shot progression
	auto TargetProgress = [this]()
	{
		const float AimScale = bIsAiming ? RecoilData->ProgressAimAlpha : 1.f;
		PitchProgress.Y += RecoilData->PitchProgress.Amount * InputScale.Progression.X * AimScale;
		ZProgress.Y += RecoilData->ZProgress.Amount * InputScale.Progression.Y * AimScale;
	};

	// Select hip or ADS profiles
	if (bIsAiming)
	{
		RandomRot(RecoilData->RollAim, RecoilData->PitchAim, RecoilData->YawAim);
		RandomLoc(RecoilData->KickAim, RecoilData->KickAimR, RecoilData->KickAimUp);
	}
	else
	{
		RandomRot(RecoilData->RollHip, RecoilData->PitchHip, RecoilData->YawHip);
		RandomLoc(RecoilData->KickHip, RecoilData->KickHipR, RecoilData->KickHipUp);
	}

	// Sway accumulation
	float SwayValue = FMath::FRandRange(RecoilData->RecoilSway.PitchSway.X, RecoilData->RecoilSway.PitchSway.Y);
	if (bIsAiming) { SwayValue *= RecoilData->RecoilSway.AdsScale; }
	PitchSway.Y += SwayValue;

	SwayValue = FMath::FRandRange(RecoilData->RecoilSway.YawSway.X, RecoilData->RecoilSway.YawSway.Y);
	if (bIsAiming) { SwayValue *= RecoilData->RecoilSway.AdsScale; }
	YawSway.Y += SwayValue;

	RandomNoise();
	TargetProgress();
}

// ---------------------------------------------------------------
//  CorrectStart — blend the start value to zero when the curve
//  passes its peak and begins descending.
// ---------------------------------------------------------------

double URecoilComponent::CorrectStart(double& Last, const double Current, bool& bStartRest, double& StartVal)
{
	if (FMath::Abs(Last) > FMath::Abs(Current) && bStartRest && !bCurveLooping)
	{
		StartVal   = 0.0;
		bStartRest = false;
	}

	Last = Current;
	return StartVal;
}

// ---------------------------------------------------------------
//  SetupTransition — arm a new curve pair and start the playhead
// ---------------------------------------------------------------

void URecoilComponent::SetupTransition(const FRotator StartRot, const FVector StartLoc, UCurveVector* Rot, UCurveVector* Loc)
{
	if (!(Rot && Loc))
	{
		UE_LOG(LogRecoil, Error, TEXT("SetupTransition: Rot or Loc curve is null."));
		return;
	}

	StartValRot = StartRot;
	StartValLoc = StartLoc;

	CanRestRot = CanRestLoc = FRecoilStartRest(true, true, true);

	TempRot = Rot;
	TempLoc = Loc;

	// Derive the natural curve length from its last keyframe
	float MinTime, MaxTime;
	TempRot->GetTimeRange(MinTime, MaxTime);
	CurveLength   = MaxTime;
	CurveTime     = 0.f;
	bCurvePlaying = true;
	// bCurveLooping is set by the calling state (Auto OnPlay), not here
}

// ---------------------------------------------------------------
//  CorrectAlpha — reseed start values at a future playback position
//  so Semi → Auto transitions blend seamlessly.
// ---------------------------------------------------------------

void URecoilComponent::CorrectAlpha(const UCurveVector* Rotation, const UCurveVector* Location, const float TimeCorrection)
{
	const FVector RotAlpha = Rotation->GetVectorValue(TimeCorrection);
	const FVector LocAlpha = Location->GetVectorValue(TimeCorrection);

	StartValRot.Pitch = FMath::Lerp(StartValRot.Pitch, TargetRot.Pitch, RotAlpha.Y);
	StartValRot.Yaw = FMath::Lerp(StartValRot.Yaw,   TargetRot.Yaw,   RotAlpha.Z);
	StartValRot.Roll = FMath::Lerp(StartValRot.Roll,  TargetRot.Roll,  RotAlpha.X);
	StartValLoc = FMath::Lerp(StartValLoc, TargetLoc, LocAlpha);
}

// ---------------------------------------------------------------
//  GetAngularRecoil — pick from min or max random range
// ---------------------------------------------------------------

double URecoilComponent::GetAngularRecoil(const FVector4 RecoilProfile)
{
	const bool bPickMax = UKismetMathLibrary::RandomBool();
	const double Max = FMath::FRandRange(RecoilProfile.Z, RecoilProfile.W);
	const double Min = FMath::FRandRange(RecoilProfile.X, RecoilProfile.Y);
	return bPickMax ? Max : Min;
}

// ---------------------------------------------------------------
//  Apply layers
// ---------------------------------------------------------------

void URecoilComponent::ApplySmoothing(float DeltaTime)
{
	if (bEnableSmoothing)
	{
		auto ApplyInterp = [&](float Raw, float Out, float Smooth) -> float
		{
			if (FMath::IsNearlyZero(Smooth, 0.01f))
				return Raw;

			// When the curve has finished, blend toward zero rather than the raw value
			Raw = bCurvePlaying ? Raw : 0.f;
			return GLerp(Out, Raw, Smooth * 2.f, DeltaTime);
		};

		LerpedRot.Pitch = ApplyInterp(RawOutRot.Pitch, LerpedRot.Pitch, RecoilData->SmoothRot.Pitch);
		LerpedRot.Yaw = ApplyInterp(RawOutRot.Yaw,   LerpedRot.Yaw,   RecoilData->SmoothRot.Yaw);
		LerpedRot.Roll = ApplyInterp(RawOutRot.Roll,  LerpedRot.Roll,  RecoilData->SmoothRot.Roll);

		LerpedLoc.X = ApplyInterp(RawOutLoc.X, LerpedLoc.X, RecoilData->SmoothLoc.X);
		LerpedLoc.Y = ApplyInterp(RawOutLoc.Y, LerpedLoc.Y, RecoilData->SmoothLoc.Y);
		LerpedLoc.Z = ApplyInterp(RawOutLoc.Z, LerpedLoc.Z, RecoilData->SmoothLoc.Z);
	}
	else
	{
		LerpedRot = RawOutRot;
		LerpedLoc = RawOutLoc;
	}
}

void URecoilComponent::ApplyNoise(FTransform& Finalized, float DeltaTime)
{
	Noise2DTarget.X = GLerp(Noise2DTarget.X, 0.f, RecoilData->Noise2DZero.X, DeltaTime);
	Noise2DTarget.Y = GLerp(Noise2DTarget.Y, 0.f, RecoilData->Noise2DZero.Y, DeltaTime);

	Noise2DCurrent.X = GLerp(Noise2DCurrent.X, Noise2DTarget.X, NoiseSpeed.X, DeltaTime);
	Noise2DCurrent.Y = GLerp(Noise2DCurrent.Y, Noise2DTarget.Y, NoiseSpeed.Y, DeltaTime);

	const float NoiseScalar = bIsAiming ? RecoilData->NoiseAimScalar : 1.f;

	const FVector NoiseLoc = Finalized.GetLocation() + FVector(0.0, Noise2DCurrent.X * NoiseScalar, Noise2DCurrent.Y * NoiseScalar);

	Finalized.SetLocation(NoiseLoc);
}

void URecoilComponent::ApplyPushback(FTransform& Finalized, float DeltaTime)
{
	PushbackTarget = GLerp(PushbackTarget,  0.0, RecoilData->PushbackZero, DeltaTime);
	PushbackCurrent = GLerp(PushbackCurrent, PushbackTarget, PushbackSpeed,  DeltaTime);

	Finalized.SetLocation(Finalized.GetLocation() + FVector(PushbackCurrent, 0.0, 0.0));
}

void URecoilComponent::ApplyProgression(FTransform& Finalized, float DeltaTime)
{
	PitchProgress.X = GLerp(PitchProgress.X, PitchProgress.Y, PitchProgressSpeed, DeltaTime);
	PitchProgress.Y = GLerp(PitchProgress.Y, 0.f, RecoilData->PitchProgress.Damping, DeltaTime);

	ZProgress.X = GLerp(ZProgress.X, ZProgress.Y, ZProgressSpeed, DeltaTime);
	ZProgress.Y = GLerp(ZProgress.Y, 0.f, RecoilData->ZProgress.Damping, DeltaTime);

	const FVector Loc = Finalized.GetLocation() + FVector(0.0, 0.0, ZProgress.X);
	const FQuat Rot = Finalized.GetRotation() * FRotator(PitchProgress.X, 0.0, 0.0).Quaternion();

	Finalized.SetLocation(Loc);
	Finalized.SetRotation(Rot);
}

void URecoilComponent::ApplyRecoilSway(FTransform& Finalized, float DeltaTime)
{
	PitchSway.X = GLerp(PitchSway.X, PitchSway.Y, RecoilData->RecoilSway.Acceleration, DeltaTime);
	YawSway.X = GLerp(YawSway.X, YawSway.Y, RecoilData->RecoilSway.Acceleration, DeltaTime);

	PitchSway.Y = GLerp(PitchSway.Y, 0.f, RecoilData->RecoilSway.Damping, DeltaTime);
	YawSway.Y = GLerp(YawSway.Y,   0.f, RecoilData->RecoilSway.Damping, DeltaTime);

	const FRotator SwayRot(PitchSway.X, YawSway.X, YawSway.X * RecoilData->RecoilSway.RollSwayMultiplier);

	const FVector SwayPos = SwayRot.RotateVector(RecoilData->RecoilSway.PivotOffset) - RecoilData->RecoilSway.PivotOffset;

	Finalized.SetRotation(Finalized.GetRotation() * SwayRot.Quaternion());
	Finalized.AddToTranslation(SwayPos);
}

// ---------------------------------------------------------------
//  Controller recoil helpers
// ---------------------------------------------------------------

void URecoilComponent::SetControllerRecoilTarget()
{
	if (!IsFiring)
		Compensation = FVector2D::ZeroVector;

	IsFiring = true;

	TargetRecoil.X += FMath::FRandRange(RecoilData->ControllerRecoil.HorizontalRecoilStep.X, RecoilData->ControllerRecoil.HorizontalRecoilStep.Y);
	TargetRecoil.Y -= FMath::FRandRange(RecoilData->ControllerRecoil.VerticalRecoilStep.X, RecoilData->ControllerRecoil.VerticalRecoilStep.Y);

	TargetRecoil.X = FMath::Clamp(TargetRecoil.X, -90.f, 90.f);
	TargetRecoil.Y = FMath::Clamp(TargetRecoil.Y, -90.f, 90.f);
}

void URecoilComponent::RefreshControllerCompensation()
{
	IsFiring = false;

	const float Weight = RecoilData->ControllerRecoil.Compensation;

	const float CompX = ApplyCompensation(Recoil.X,  Compensation.X * Weight);
	const float CompY = ApplyCompensation(Recoil.Y, -Compensation.Y * Weight);

	Recoil.X *= CompX;
	Recoil.Y *= CompY;
	TargetRecoil = Recoil;
}

float URecoilComponent::ApplyCompensation(const float RecoilValue, const float RecoilCompensation) const
{
	float Multiplier = 1.f;
	const bool bIsOpposite = RecoilValue * RecoilCompensation <= 0.f;

	if (!FMath::IsNearlyZero(RecoilCompensation) && bIsOpposite)
		Multiplier -= FMath::Clamp(FMath::Abs(RecoilCompensation / RecoilValue), 0.f, 1.f);

	return Multiplier;
}

// ---------------------------------------------------------------
//  GLerp overloads
// ---------------------------------------------------------------

FVector URecoilComponent::GLerp(FVector A, FVector B, FVector Alpha, float DeltaTime) const
{
	return FVector(GLerp(A.X, B.X, Alpha.X, DeltaTime), GLerp(A.Y, B.Y, Alpha.Y, DeltaTime), GLerp(A.Z, B.Z, Alpha.Z, DeltaTime));
}

FRotator URecoilComponent::GLerp(FRotator A, FRotator B, FVector Alpha, float DeltaTime) const
{
	return FRotator(GLerp(A.Pitch, B.Pitch, Alpha.Y, DeltaTime), GLerp(A.Yaw, B.Yaw, Alpha.Z, DeltaTime), GLerp(A.Roll, B.Roll, Alpha.X, DeltaTime));
}

// ---------------------------------------------------------------
//  DampSpring — reserved; not called in the active code path
// ---------------------------------------------------------------

float URecoilComponent::DampSpring(float Current, FFloatSpringState& SpringState, float Stiffness, float Damping, float DeltaTime)
{
	DeltaTime = FMath::Min(DeltaTime, 1.f);
	return UKismetMathLibrary::FloatSpringInterp(Current, 0.f, SpringState, Stiffness, Damping, DeltaTime);
}
