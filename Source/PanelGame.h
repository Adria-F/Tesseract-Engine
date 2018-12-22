#ifndef __PANELGAME_H__
#define __PANELGAME_H__

#include "Panel.h"

class PanelGame :public Panel
{
public:
	PanelGame(const char* name);
	~PanelGame();

	void Draw();

public:

	ImVec2 size = { 0,0 };
	ImVec2 viewSize = { 0,0 };
	bool resizedLastFrame = false;
	float lastAR = 0;
};

#endif // !__PANELGAME_H__
