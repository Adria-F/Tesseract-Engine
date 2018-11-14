#ifndef __PANELTIMEMANAGER_H__
#define __PANELTIMEMANAGER_H__

#include "Panel.h"

class PanelTimeManager : public Panel
{
public:
	PanelTimeManager(const char* name);
	~PanelTimeManager();

	void Draw();
};

#endif // !__PANELTIMEMANAGER_H__
