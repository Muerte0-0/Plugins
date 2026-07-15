// Copyright Wonderveil Studios 2025 - 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "Data/RecoilData.h"
#include "GameFramework/Pawn.h"
#include "RecoilComponent.generated.h"

// ---------------------------------------------------------------
//  Log category
// ---------------------------------------------------------------

DECLARE_LOG_CATEGORY_EXTERN(LogRecoil, Log, All);

// ---------------------------------------------------------------
//  Internal delegate types — state machine plumbing
// ---------------------------------------------------------------

DECLARE_DELEGATE(FDoTransition);
DECLARE_DELEGATE_RetVal(bool, FCheckCondition);

// ---------------------------------------------------------------
//  EFireMode
// ---------------------------------------------------------------

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	Semi   UMETA(DisplayName = "Semi"),
	Auto   UMETA(DisplayName = "Auto"),
	Burst  UMETA(DisplayName = "Burst"),
};

// ---------------------------------------------------------------
//  FRecoilMuteAxis
//  Selectively silence individual output axes.
// ---------------------------------------------------------------

USTRUCT(BlueprintType)
struct FPFRAMEWORK_API FRecoilMuteAxis
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Recoil")
	bool X = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Recoil")
	bool Y = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Recoil")
	bool Z = false;
};

// ---------------------------------------------------------------
//  FRecoilInputScale
//  Per-axis scale factors applied before target randomisation.
//  ScaleInput / UnscaleInput accumulate these multiplicatively.
// ---------------------------------------------------------------

USTRUCT(BlueprintType)
struct FPFRAMEWORK_API FRecoilInputScale
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	FVector2D Pitch = FVector2D::UnitVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	FVector2D Kick = FVector2D::UnitVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	FVector2D KickR = FVector2D::UnitVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	FVector2D KickUp = FVector2D::UnitVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	FVector4 Yaw = FVector4(1, 1, 1, 1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	FVector4 Roll = FVector4(1, 1, 1, 1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	FVector2D Noise = FVector2D::UnitVector;

	/** X = Pitch progression scale, Y = Z progression scale */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	FVector2D Progression = FVector2D::UnitVector;

	FRecoilInputScale operator*(const FRecoilInputScale& A) const
	{
		FRecoilInputScale Out;
		Out.Pitch       = Pitch       * A.Pitch;
		Out.Kick        = Kick        * A.Kick;
		Out.KickR       = KickR       * A.KickR;
		Out.KickUp      = KickUp      * A.KickUp;
		Out.Yaw         = Yaw         * A.Yaw;
		Out.Roll        = Roll        * A.Roll;
		Out.Noise       = Noise       * A.Noise;
		Out.Progression = Progression * A.Progression;
		return Out;
	}

	void operator*=(const FRecoilInputScale& A)
	{
		Pitch       *= A.Pitch;
		Kick        *= A.Kick;
		KickR       *= A.KickR;
		KickUp      *= A.KickUp;
		Yaw         *= A.Yaw;
		Roll        *= A.Roll;
		Noise       *= A.Noise;
		Progression *= A.Progression;
	}

	FRecoilInputScale operator/(const FRecoilInputScale& A) const
	{
		FRecoilInputScale Out;
		Out.Pitch       = Pitch       / A.Pitch;
		Out.Kick        = Kick        / A.Kick;
		Out.KickR       = KickR       / A.KickR;
		Out.KickUp      = KickUp      / A.KickUp;
		Out.Yaw         = Yaw         / A.Yaw;
		Out.Roll        = Roll        / A.Roll;
		Out.Noise       = Noise       / A.Noise;
		Out.Progression = Progression / A.Progression;
		return Out;
	}

	void operator/=(const FRecoilInputScale& A)
	{
		Pitch       /= A.Pitch;
		Kick        /= A.Kick;
		KickR       /= A.KickR;
		KickUp      /= A.KickUp;
		Yaw         /= A.Yaw;
		Roll        /= A.Roll;
		Noise       /= A.Noise;
		Progression /= A.Progression;
	}

	void Reset()
	{
		Pitch = Kick = KickR = KickUp = Noise = Progression = FVector2D::UnitVector;
		Yaw = Roll = FVector4(1, 1, 1, 1);
	}
};

// ---------------------------------------------------------------
//  Internal state machine types — not exposed publicly
// ---------------------------------------------------------------

USTRUCT()
struct FRecoilStartRest
{
	GENERATED_BODY()

	FRecoilStartRest() = default;
	FRecoilStartRest(bool InX, bool InY, bool InZ) : X(InX), Y(InY), Z(InZ) {}

	bool X = false;
	bool Y = false;
	bool Z = false;
};

USTRUCT()
struct FRecoilAnimState
{
	GENERATED_BODY()

	/** Returns true if this state may begin its transition */
	FCheckCondition TransitionCondition;

	/** Executed inside Play() when this state is selected */
	FDoTransition OnPlay;

	/** Executed inside Stop() */
	FDoTransition OnStop;
};

// ---------------------------------------------------------------
//  URecoilComponent
//
//  Procedural recoil animation component. Attach to any APawn.
//
//  Usage:
//    1. Call Init(RecoilData, FireRate, BurstAmount) when the
//       weapon is equipped / fire mode is configured.
//    2. Call Play() on each shot trigger pull.
//    3. Call Stop() when the trigger is released (Auto/Burst).
//    4. Feed GetOutput() (FTransform) into the anim graph via
//       FAnimNode_Recoil or manually.
//
//  Replication:
//    OutRot and OutLoc replicate with COND_SkipOwner — the owning
//    client simulates locally; non-owning clients/server receive
//    the result for cosmetic playback.
// ---------------------------------------------------------------

UCLASS(ClassGroup = (WonderveilStudios), meta = (BlueprintSpawnableComponent))
class FPFRAMEWORK_API URecoilComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	URecoilComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// -----------------------------------------------------------
	//  Public API
	// -----------------------------------------------------------

	/** Initialize the component for a weapon configuration.
	 *  Call this whenever the equipped weapon or fire mode changes.
	 *  @param NewRecoilData Data Asset with all tuning values
	 *  @param Rate Fire rate in shots per minute (Auto/Burst)
	 *  @param Bursts Burst count (0 = ignored for Semi/Auto)
	 */
	UFUNCTION(BlueprintCallable, Category = "Recoil")
	void Init(URecoilData* NewRecoilData, float Rate = 0.f, int32 Bursts = 0);

	/** Fire — call once per shot (Semi) or on trigger press (Auto) */
	UFUNCTION(BlueprintCallable, Category = "Recoil")
	void Play();

	/** Release trigger — call when Auto/Burst fire ends */
	UFUNCTION(BlueprintCallable, Category = "Recoil")
	void Stop();

	/** Replaces the active fire mode.  Call before Init() or between bursts. */
	UFUNCTION(BlueprintCallable, Category = "Recoil")
	void SetFireMode(EFireMode NewMode);

	/** Notify the component whether the pawn is aiming down sights */
	UFUNCTION(BlueprintCallable, Category = "Recoil")
	void SetAimingStatus(bool bStatus);

	/** Multiply the current InputScale by NewScale (e.g. for attachments) */
	UFUNCTION(BlueprintCallable, Category = "Recoil")
	void ScaleInput(FRecoilInputScale NewScale);

	/** Divide the current InputScale by NewScale — undoes a prior ScaleInput */
	UFUNCTION(BlueprintCallable, Category = "Recoil")
	void UnscaleInput(FRecoilInputScale NewScale);

	/** Reset InputScale to identity */
	UFUNCTION(BlueprintCallable, Category = "Recoil")
	void ResetInputScale();

	/** Final recoil transform to feed into the anim graph */
	FORCEINLINE FTransform GetOutput() const { return FTransform(OutRot, OutLoc); }

	/** Seconds since the last Play() call */
	UFUNCTION(BlueprintCallable, Category = "Recoil")
	float GetDelta() const;

	UFUNCTION(BlueprintPure, Category = "Recoil")
	FORCEINLINE EFireMode GetFireMode() const { return FireMode; }

	// -----------------------------------------------------------
	//  Exposed tunables
	// -----------------------------------------------------------

	/** Silence individual rotation output axes */
	UPROPERTY(BlueprintReadWrite, Category = "Recoil")
	FRecoilMuteAxis MuteRot;

	/** Silence individual translation output axes */
	UPROPERTY(BlueprintReadWrite, Category = "Recoil")
	FRecoilMuteAxis MuteLoc;

	/** Replicated so proxies can read the scale set by the owning client */
	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Recoil")
	FRecoilInputScale InputScale;

	UPROPERTY(BlueprintReadOnly, Category = "Recoil")
	TObjectPtr<URecoilData> RecoilData;

	/** Global translation multiplier applied after all layers */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil")
	FVector GlobalTranslationMultiplier = FVector::OneVector;

	/** Global rotation multiplier applied after all layers */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil")
	FVector GlobalRotationMultiplier = FVector::OneVector;

protected:

	virtual void BeginPlay() override;

private:

	// -----------------------------------------------------------
	//  Core solver
	// -----------------------------------------------------------

	void SetupComponent();
	void SetupStateMachine();

	void TickAdvancedSolver(float DeltaTime);
	void TickControllerRecoil(float DeltaTime);

	void AnimationUpdate();
	void CalculateTargetData();

	void SetupTransition(FRotator StartRot, FVector StartLoc, UCurveVector* Rot, UCurveVector* Loc);
	void CorrectAlpha(const UCurveVector* Rotation, const UCurveVector* Location, float TimeCorrection);

	double CorrectStart(double& Last, double Current, bool& bStartRest, double& StartVal);

	double GetAngularRecoil(FVector4 RecoilProfile);

	// -----------------------------------------------------------
	//  Apply layers
	// -----------------------------------------------------------

	void ApplySmoothing(float DeltaTime);
	void ApplyNoise(FTransform& Finalized, float DeltaTime);
	void ApplyPushback(FTransform& Finalized, float DeltaTime);
	void ApplyProgression(FTransform& Finalized, float DeltaTime);
	void ApplyRecoilSway(FTransform& Finalized, float DeltaTime);

	// -----------------------------------------------------------
	//  Controller recoil helpers
	// -----------------------------------------------------------

	void SetControllerRecoilTarget();
	void RefreshControllerCompensation();
	float ApplyCompensation(float RecoilValue, float RecoilCompensation) const;

	// -----------------------------------------------------------
	//  GLerp — frame-rate-independent interpolation
	// -----------------------------------------------------------

	FORCEINLINE double GLerp(double A, double B, double Alpha, double DeltaTime) const
	{
		return FMath::Lerp(A, B, 1.0 - FMath::Exp(-Alpha * DeltaTime));
	}

	FVector GLerp(FVector  A, FVector  B, FVector Alpha, float DeltaTime) const;
	FRotator GLerp(FRotator A, FRotator B, FVector Alpha, float DeltaTime) const;

	// -----------------------------------------------------------
	//  Spring (declared for completeness; reserved for future use)
	// -----------------------------------------------------------

	float DampSpring(float Current, FFloatSpringState& SpringState, float Stiffness, float Damping, float DeltaTime);

	// -----------------------------------------------------------
	//  Utility
	// -----------------------------------------------------------

	FORCEINLINE double GetNonZero(double OldValue, double NewValue) const
	{
		return FMath::IsNearlyZero(NewValue) ? OldValue : NewValue;
	}

	// -----------------------------------------------------------
	//  Replicated output
	// -----------------------------------------------------------

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Recoil Output", meta = (AllowPrivateAccess = true))
	FRotator OutRot = FRotator::ZeroRotator;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Recoil Output", meta = (AllowPrivateAccess = true))
	FVector OutLoc = FVector::ZeroVector;

	// -----------------------------------------------------------
	//  Curve pointers (set by SetupTransition)
	// -----------------------------------------------------------

	UPROPERTY()
	TObjectPtr<UCurveVector> TempRot;

	UPROPERTY()
	TObjectPtr<UCurveVector> TempLoc;

	// -----------------------------------------------------------
	//  Playhead state  (replaces FTimeline)
	// -----------------------------------------------------------

	float CurveTime = 0.f;   // current position along the active curve
	float CurveLength = 0.f;   // end time derived from last keyframe or 60/FireRate
	float CurvePlayRate = 1.f;   // mirrors RecoilData->PlayRate
	bool bCurvePlaying = false;
	bool bCurveLooping = false;

	// -----------------------------------------------------------
	//  State machine
	// -----------------------------------------------------------

	TArray<FRecoilAnimState> StateMachine;
	uint8 CurrentStateIndex = 0;

	// -----------------------------------------------------------
	//  Solver state
	// -----------------------------------------------------------

	FRecoilStartRest CanRestRot;
	FRecoilStartRest CanRestLoc;

	FRotator StartValRot = FRotator::ZeroRotator;
	FVector StartValLoc = FVector::ZeroVector;
	FRotator TargetRot = FRotator::ZeroRotator;
	FVector TargetLoc  = FVector::ZeroVector;

	FFloatSpringState PitchState;
	FFloatSpringState RollState;
	FFloatSpringState KickState;

	FVector2D Noise2DTarget = FVector2D::ZeroVector;
	FVector2D Noise2DCurrent = FVector2D::ZeroVector;
	FVector2D NoiseSpeed = FVector2D::ZeroVector;

	double PushbackTarget  = 0.0;
	double PushbackCurrent = 0.0;
	double PushbackOut = 0.0;
	double PushbackSpeed = 0.0;

	FRotator LerpedRot = FRotator::ZeroRotator;
	FVector LerpedLoc = FVector::ZeroVector;
	FRotator RawOutRot = FRotator::ZeroRotator;
	FVector RawOutLoc = FVector::ZeroVector;

	FVector2D PitchProgress = FVector2D::ZeroVector;
	FVector2D ZProgress = FVector2D::ZeroVector;
	double PitchProgressSpeed = 0.0;
	double ZProgressSpeed = 0.0;

	FVector2D PitchSway = FVector2D::ZeroVector;
	FVector2D YawSway = FVector2D::ZeroVector;

	// -----------------------------------------------------------
	//  Controller recoil state
	// -----------------------------------------------------------

	FVector2D Compensation = FVector2D::ZeroVector;
	FVector2D TargetRecoil = FVector2D::ZeroVector;
	FVector2D Recoil = FVector2D::ZeroVector;
	bool IsFiring = false;

	// -----------------------------------------------------------
	//  Misc
	// -----------------------------------------------------------

	EFireMode FireMode = EFireMode::Semi;
	float FireRate = 600.f;
	int32 BurstAmount = 0;
	int32 BurstCounter = 0;

	float LastShotTime = 0.f;
	bool bIsAiming = false;
	bool bAnimActive = false;
	bool bEnableSmoothing = false;

	UPROPERTY()
	TObjectPtr<APawn> OwnerCharacter;
};
