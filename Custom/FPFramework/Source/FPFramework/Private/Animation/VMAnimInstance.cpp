// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/VMAnimInstance.h"
#include "GameFramework/Pawn.h"

void UVMAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (APawn* OwnerPawn = TryGetPawnOwner())
		ViewmodelController = OwnerPawn->GetComponentByClass<UViewmodelController>();
}

void UVMAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!ViewmodelController) return;

	AnimState = ViewmodelController->GetAnimState();
}