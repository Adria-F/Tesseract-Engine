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
	bool resizedLastFrame = false;

private:

	float gametimer = 0.0f;
	bool gamePaused = false;
	bool step = false;

};

#endif // !__PANELGAME_H__
