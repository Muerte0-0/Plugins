// 2025 - 2026 Wonderveil Studios, Wonderveil Studios, Nexus Interface and any associated logos are trademarks, service marks, and/or registered trademarks of Wonderveil Studios.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NexusDisplayFunctionLibrary.generated.h"

USTRUCT(BlueprintType)
struct FMonitorDisplayInfo
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	FString Name;
	
	UPROPERTY(BlueprintReadOnly)
	FString ID;
	
	UPROPERTY(BlueprintReadOnly)
	int32 NativeWidth = 0;
	
	UPROPERTY(BlueprintReadOnly)
	int32 NativeHeight = 0;
	
	UPROPERTY(BlueprintReadOnly)
	FIntPoint MaxResolution = FIntPoint(ForceInitToZero);
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsPrimary = false;
	
	UPROPERTY(BlueprintReadOnly)
	int32 DPI = 0;
	
	FMonitorDisplayInfo()
	{
		Name = FString();
		ID = FString();
		NativeHeight = 0;
		NativeWidth = 0;
		MaxResolution = FIntPoint(ForceInitToZero);
		bIsPrimary = false;
		DPI = 0;
	}
	
	FMonitorDisplayInfo(FString name, FString id, int32 width, int32 height, FIntPoint maxRes, bool isPrimary, int32 dpi) : 
	Name(name), ID(id), NativeWidth(width), NativeHeight(height), MaxResolution(maxRes), bIsPrimary(isPrimary), DPI(dpi)
	{}
};

/**
 * 
 */
UCLASS()
class NEXUSINTERFACE_API UNexusDisplayFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category="Nexus Interface|Display")
	static TArray<FMonitorDisplayInfo> GetAllMonitorInfo();
	
	UFUNCTION(BlueprintCallable, Category ="Nexus Interface|Display")
	static bool SetActiveDisplay(int Index);
};
