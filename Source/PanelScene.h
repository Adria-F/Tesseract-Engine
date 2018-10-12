#ifndef __PANELSCENE_H__
#define __PANELSCENE_H__

#include "Panel.h"

class PanelScene:public Panel
{
public:
	PanelScene(const char* name);
	~PanelScene();

	void Draw();

};

#endif // !__PANELSCENE_H__
