#ifndef __PANELPROPERTIES_H__
#define __PANELPROPERTIES_H__

#include "Panel.h"

class PanelProperties : public Panel
{
public:
	PanelProperties(const char* name, float posX, float posY, float width, float height, panelAlingnment aligned = NO_ALINGNMENT);
	~PanelProperties();

	void Draw();
};

#endif // !__PANELPROPERTIES_H__