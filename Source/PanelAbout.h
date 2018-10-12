#ifndef __PANELABOUT_H__
#define __PANELABOUT_H__

#include "Panel.h"

class PanelAbout : public Panel
{
public:
	PanelAbout(const char* name);
	~PanelAbout();

	void Draw();
};

#endif // !__PANELABOUT_H__