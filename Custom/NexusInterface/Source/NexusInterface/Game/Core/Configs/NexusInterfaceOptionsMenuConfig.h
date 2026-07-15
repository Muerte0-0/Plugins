// 2025 - 2026 Wonderveil Studios, Wonderveil Studios, Nexus Interface and any associated logos are trademarks, service marks, and/or registered trademarks of Wonderveil Studios.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "NexusInterface/Game/OptionDefinition.h"
#include "NexusInterface/UI/Presets/BaseOptionsPanel.h"

#include "NexusInterfaceOptionsMenuConfig.generated.h"

USTRUCT(BlueprintType)
struct FOptionsData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nexus Interface|Options")
	FText Category = FText::FromString("None");
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nexus Interface|Options")
	TArray<TObjectPtr<UOptionDefinition>> Definitions;
};

USTRUCT(BlueprintType)
struct FPanelData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nexus Interface|Options")
	FName PanelName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nexus Interface|Options")
	TSubclassOf<UBaseOptionsPanel> PanelWidgetClass;
	
	// Leave CategoryName as None to Hide Category Name
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nexus Interface|Options")
	TArray<FOptionsData> Options;
};

/**
 * 
 */
UCLASS()
class NEXUSINTERFACE_API UNexusInterfaceOptionsMenuConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nexus Interface|Options")
	TArray<FPanelData> PanelData;
	
	UFUNCTION(BlueprintCallable, Category = "Nexus Interface|Options")
	FPanelData GetPanelData(FName Panel);
};
