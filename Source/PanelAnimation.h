#ifndef __PANELANIMATION_H__
#define __PANELANIMATION_H__

#include "Panel.h"

class PanelAnimation :
	public Panel
{
public:

	PanelAnimation(const char* name);
	~PanelAnimation();

	void Draw();
};

#endif // !__PANELANIMATION_H__