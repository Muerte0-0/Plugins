// 2025 - 2026 Wonderveil Studios, Wonderveil Studios, Nexus Interface and any associated logos are trademarks, service marks, and/or registered trademarks of Wonderveil Studios.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BaseOptionsPanel.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, HideDropdown)
class NEXUSINTERFACE_API UBaseOptionsPanel : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nexus Interface")
	FName PanelName;
	
	virtual void NativePreConstruct() override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Nexus Interface")
	void ResetOptions();
	virtual void ResetOptions_Implementation() {}
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Nexus Interface")
	void ApplyChanges();
	virtual void ApplyChanges_Implementation() {}
		
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Nexus Interface")
	void RevertChanges();
	virtual void RevertChanges_Implementation() {}
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Nexus Interface")
	bool HasUnsavedChanges();
	virtual bool HasUnsavedChanges_Implementation() { return false; }
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure, Category = "Nexus Interface")
	TArray<FOptionsData> GetMenuOptionsData();
	virtual TArray<FOptionsData> GetMenuOptionsData_Implementation();
};
