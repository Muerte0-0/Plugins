// Copyright Guilty Saint Games 2026, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "AnimGraphNode_BlendListBase.h"
#include "AnimNode_BlendListByGameplayTag.h"

#include "AnimGraphNode_BlendListByGameplayTag.generated.h"

class UGraphNodeContextMenuContext;
class UToolMenu;

UCLASS()
class BLENDPOSESBYTAGEDITOR_API UAnimGraphNode_BlendListByGameplayTag : public UAnimGraphNode_BlendListBase
{
	GENERATED_UCLASS_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category=Settings)
	FAnimNode_BlendListByGameplayTag Node;

	TSharedPtr<SWindow> GameplayTagSelectionWindow;

	// Gameplay Tag Selection
	void OpenGameplayTagSelectionWindow();
	void OnGameplayTagSelectionChanged(const FGameplayTagContainer& Tags);

	// Adding/Removing pose pins from the node (Context Menu Actions)
	void AddBlendPinForGameplayTag(const FGameplayTag& TagToExpose);
	void RemoveBlendPinForGameplayTag(const FGameplayTag& TagToRemove);
	void RemoveBlendPin(const UEdGraphPin* Pin);
	void RemoveAllBlendPins();

	// Context Menu Actions
	void AddContextMenuAction_SelectGameplayTags(UToolMenu* Menu, FToolMenuSection* Section) const;
	void AddContextMenuAction_RemoveBlendPin(UToolMenu* Menu, const UEdGraphPin* Pin) const;
	void AddContextMenuAction_RemoveAllBlendPins(UToolMenu* Menu, FToolMenuSection* Section) const;

	static FString GetGameplayTagLeafName(const FGameplayTag& Tag);

public:
	// UEdGraphNode Base interface
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 ArrayIndex) const override;
	// End UEdGraphNode Base interface

	// UK2Node interface
	virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;
	// End UK2Node interface
};

