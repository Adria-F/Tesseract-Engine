#ifndef __PANELABOUT_H__
#define __PANELABOUT_H__

#include "Panel.h"

class PanelAbout : public Panel
{
public:
	PanelAbout(const char* name, float posX, float posY, float width, float height);
	~PanelAbout();

	void Draw();
};

#endif // !__PANELABOUT_H__