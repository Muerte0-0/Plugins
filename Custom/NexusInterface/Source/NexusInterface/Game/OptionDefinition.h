// 2025 - 2026 Wonderveil Studios, Wonderveil Studios, Nexus Interface and any associated logos are trademarks, service marks, and/or registered trademarks of Wonderveil Studios.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "OptionDefinition.generated.h"

UENUM(BlueprintType)
enum class EOptionType : uint8
{
	Cycle,
	Toggle,
	Dropdown,
	Slider,
	KeySwap,
};

/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew, Abstract, DefaultToInstanced, HideDropdown)
class NEXUSINTERFACE_API UOptionDefinition : public UObject
{
	GENERATED_BODY()
	
private:
	TWeakObjectPtr<UWorld> WorldPtr;
	
public:
	virtual UWorld* GetWorld() const override { return WorldPtr.IsValid() && WorldPtr->IsValidLowLevelFast() ? WorldPtr.Get() : nullptr; }
	void SetWorld(UWorld* WorldReference) { WorldPtr = WorldReference; }
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name = FText::FromString("None");
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Description = FText::FromString("None");
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UTexture2D> PreviewImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EOptionType Type = EOptionType::Cycle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (InlineEditConditionToggle))
	bool bUseCustomOptions = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bUseCustomOptions"))
	TArray<FText> Options;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "Type == EOptionType::Slider"))
	FVector2D MinMaxSliderValue = FVector2d(0.0f, 100.0f);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "Type == EOptionType::Slider"))
	float SliderStepSize = 0.01;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void InitOptions();
	virtual void InitOptions_Implementation() {}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent)
	TArray<FText> GetOptions();
	virtual TArray<FText> GetOptions_Implementation() { return Options; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent)
	int GetCurrentValue(float& Value);
	int GetCurrentValue_Implementation(float& Value) { Value = 1.0f; return 0; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent)
	FText GetOptionName(int Index);
	virtual FText GetOptionName_Implementation(int Index) { return Options[Index]; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent)
    bool IsOptionSupported();
	virtual bool IsOptionSupported_Implementation() { return true; }
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnOptionValueChanged(int Index, float Value);
	virtual void OnOptionValueChanged_Implementation(int Index, float Value) {}
};
