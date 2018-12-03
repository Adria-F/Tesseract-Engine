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

	ImVec2 barMovement = { 0, 0 };
	ImVec2 mouseMovement = { 0, 0 };
	bool dragging = false;

	float recSize=700;
	float zoom = 50;
	float numFrames=1;
	float speed = 0.0f;

};

#endif // !__PANELANIMATION_H__