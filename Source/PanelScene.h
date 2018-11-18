#ifndef __PANELSCENE_H__
#define __PANELSCENE_H__

#include "Panel.h"

class PanelScene:public Panel
{
public:
	PanelScene(const char* name);
	~PanelScene();

	void Draw();

public:

	ImVec2 size = { 0,0 };
	bool resizedLastFrame = false;

private:

	float gametimer=0.0f;
	bool gamePaused = false;
	bool step = false;

};

#endif // !__PANELSCENE_H__
