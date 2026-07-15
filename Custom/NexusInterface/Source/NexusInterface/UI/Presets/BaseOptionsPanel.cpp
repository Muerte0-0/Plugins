// 2025 - 2026 Wonderveil Studios, Wonderveil Studios, Nexus Framework and any associated logos are trademarks, service marks, and/or registered trademarks of Wonderveil Studios.


#include "BaseOptionsPanel.h"

#include "NexusInterface/Game/NexusInterfaceSettings.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void UBaseOptionsPanel::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if (!GetMenuOptionsData().IsEmpty())
	{
		for (FOptionsData OptionData : GetMenuOptionsData())
		{
			for (UOptionDefinition* Option : OptionData.Definitions)
			{
				Option->SetWorld(GetWorld());
				Option->InitOptions();
			}
		}
	}
}

TArray<FOptionsData> UBaseOptionsPanel::GetMenuOptionsData_Implementation()
{
	const UNexusInterfaceSettings* NexusInterfaceSettings = GetDefault<UNexusInterfaceSettings>();;
	
	TArray<FOptionsData> Options;
	
	if (NexusInterfaceSettings->OptionsMenuConfig)
		Options = NexusInterfaceSettings->OptionsMenuConfig->GetPanelData(PanelName).Options;
	
	return Options;
}
