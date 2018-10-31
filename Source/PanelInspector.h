#ifndef __PANELINSPECTOR_H__
#define __PANELINSPECTOR_H__

#include "Panel.h"

class PanelInspector : public Panel
{
public:
	PanelInspector(const char* name);
	~PanelInspector();

	void Draw();
};

#endif // !__PANELINSPECTOR_H__