// 2025 - 2026 Wonderveil Studios, Wonderveil Studios, Nexus Interface and any associated logos are trademarks, service marks, and/or registered trademarks of Wonderveil Studios.

#include "NexusDisplayFunctionLibrary.h"

#include "GameFramework/GameUserSettings.h"

TArray<FMonitorDisplayInfo> UNexusDisplayFunctionLibrary::GetAllMonitorInfo()
{
	TArray<FMonitorDisplayInfo> DisplayInfos;
	
	FDisplayMetrics DisplayMetrics;
	FDisplayMetrics::RebuildDisplayMetrics(DisplayMetrics);
	
	for (const FMonitorInfo& MonitorInfo : DisplayMetrics.MonitorInfo)
	{
		DisplayInfos.Add(FMonitorDisplayInfo(
			MonitorInfo.Name,
			MonitorInfo.ID,
			MonitorInfo.NativeWidth,
			MonitorInfo.NativeHeight,
			MonitorInfo.MaxResolution,
			MonitorInfo.bIsPrimary,
			MonitorInfo.DPI
			));
	}
	
	return DisplayInfos;
}

bool UNexusDisplayFunctionLibrary::SetActiveDisplay(int Index)
{
	FDisplayMetrics Displays;
	FDisplayMetrics::RebuildDisplayMetrics(Displays);
	
	if (Index > Displays.MonitorInfo.Num())
		return false;
	
	const FMonitorInfo& TargetMonitor = Displays.MonitorInfo[Index];
	FVector2D NewWindowPosition(static_cast<float>(TargetMonitor.WorkArea.Left), static_cast<float>(TargetMonitor.WorkArea.Top));
	
	if (GEngine && GEngine->GameViewport)
	{
		TSharedPtr<SWindow> GWindow = GEngine->GameViewport->GetWindow();
		GWindow->MoveWindowTo(NewWindowPosition);
		return true;
	}
	
	return false;
}
