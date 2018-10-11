#ifndef __PANELHARDWAREINFO_H__
#define __PANELHARDWAREINFO_H__

#include "Panel.h"

class PanelHardwareInfo : public Panel
{
public:
	PanelHardwareInfo(const char* name, float posX, float posY, float width, float height, panelAlingnment aligned = NO_ALINGNMENT);
	~PanelHardwareInfo();

	void Draw();
};

#endif // !__PANELHARDWAREINFO_H__