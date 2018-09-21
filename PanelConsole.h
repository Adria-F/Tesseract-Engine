#ifndef __PANELCONSOLE_H__
#define __PANELCONSOLE_H__

#include "Panel.h"

class PanelConsole : public Panel
{
public:
	PanelConsole(const char* name);
	~PanelConsole();

	void Draw();
};

#endif // !__PANELCONSOLE_H__