#ifndef __PANELSCENE_H__
#define __PANELSCENE_H__

#include "Panel.h"

class PanelScene:public Panel
{
public:
	PanelScene(const char* name, float posX, float posY, float width, float height, panelAlingnment aligned = CENTER);
	~PanelScene();

	void Draw();

};

#endif // !__PANELSCENE_H__
