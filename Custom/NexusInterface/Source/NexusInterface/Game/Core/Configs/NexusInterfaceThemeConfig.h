// 2025 - 2026 Wonderveil Studios, Wonderveil Studios, Nexus Interface and any associated logos are trademarks, service marks, and/or registered trademarks of Wonderveil Studios.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "CommonBorder.h"
#include "Engine/DataAsset.h"
#include "NexusInterfaceThemeConfig.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class NEXUSINTERFACE_API UNexusInterfaceThemeConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Base|Text")
	TSubclassOf<UCommonTextStyle> DefaultTextStyle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Base|Text")
    TSubclassOf<UCommonTextStyle> SmallTextStyle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Base|Button")
	TSubclassOf<UCommonButtonStyle> ButtonStyle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Base|Button")
	TSubclassOf<UCommonTextStyle> ButtonTextStyle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Base|Button")
	FLinearColor ButtonContentHoverTint = FLinearColor::Black;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Base|Button")
	FLinearColor ButtonContentUnHoverTint = FLinearColor::White;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Confirmation Dialog")
	TSubclassOf<UCommonBorderStyle> DialogBorderStyle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Confirmation Dialog")
	TSubclassOf<UCommonTextStyle> DialogTextStyle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Confirmation Dialog")
	TSubclassOf<UCommonButtonStyle> DialogButton1Style;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Confirmation Dialog")
	TSubclassOf<UCommonButtonStyle> DialogButton2Style;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tab List|Nav Bar")
	TSubclassOf<UCommonButtonStyle> NavBarButtonStyle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tab List|Nav Bar")
	FMargin NavBarButtonPadding = FMargin(25, 0, 25, 0);
};
