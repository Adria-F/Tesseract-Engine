#ifndef __PANELPROPERTIES_H__
#define __PANELPROPERTIES_H__

#include "Panel.h"

class PanelProperties : public Panel
{
public:
	PanelProperties(const char* name);
	~PanelProperties();

	void Draw();
};

#endif // !__PANELPROPERTIES_H__