// 2025 - 2026 Wonderveil Studios, Wonderveil Studios, Nexus Interface and any associated logos are trademarks, service marks, and/or registered trademarks of Wonderveil Studios.


#include "NexusInterfaceOptionsMenuConfig.h"

FPanelData UNexusInterfaceOptionsMenuConfig::GetPanelData(FName Panel)
{
	FPanelData LCL_PanelData;
	
	for (auto panelData : PanelData)
	{
		if (panelData.PanelName == Panel)
		{
			LCL_PanelData = panelData;
			break;
		}
	}
	
	return LCL_PanelData;
}
