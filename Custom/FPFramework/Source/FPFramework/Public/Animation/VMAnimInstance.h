// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Components/ViewmodelController.h"
#include "VMAnimInstance.generated.h"

UCLASS()
class FPFRAMEWORK_API UVMAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, Category = "Viewmodel")
	FVMAnimState AnimState;
	
	UPROPERTY(BlueprintReadOnly, Category = "Viewmodel")
	TObjectPtr<UViewmodelController> ViewmodelController;
	
private:

};