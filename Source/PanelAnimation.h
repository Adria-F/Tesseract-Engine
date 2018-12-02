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

private:

	ImVec2 mouseMovement = { 0, 0 };
	bool dragging = false;
};

#endif // !__PANELANIMATION_H__