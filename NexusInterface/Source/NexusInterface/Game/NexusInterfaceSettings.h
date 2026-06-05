// 2025 - 2026 Wonderveil Studios, Wonderveil Studios, Nexus Interface and any associated logos are trademarks, service marks, and/or registered trademarks of Wonderveil Studios.

#pragma once

#include "CoreMinimal.h"
#include "Core/Configs/NexusInterfaceOptionsMenuConfig.h"
#include "Engine/DeveloperSettings.h"

#include "NexusInterface/Game/Core/Configs/NexusInterfaceThemeConfig.h"

#include "NexusInterfaceSettings.generated.h"

/**
 * 
 */
UCLASS(ClassGroup="Nexus Interface", Config=NexusFramework, DefaultConfig, meta=(DisplayName = "Nexus Interface"))
class NEXUSINTERFACE_API UNexusInterfaceSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UNexusInterfaceSettings();
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> ProjectIcon;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	FVector2D ProjectIconSize = FVector2D(800, 400);
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	FMargin ProjectIconPadding = FMargin(200, 200, 0, 0);
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UNexusInterfaceThemeConfig> ThemeConfig;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UNexusInterfaceOptionsMenuConfig> OptionsMenuConfig;
	
#if WITH_EDITOR
	
	virtual FName GetCategoryName() const override { return TEXT("Nexus Framework"); }
	
#endif
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const UNexusInterfaceSettings* GetNexusInterfaceSettings() { return GetDefault<UNexusInterfaceSettings>(); }
};
