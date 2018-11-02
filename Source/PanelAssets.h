#ifndef __PANELASSETS_H__
#define __PANELASSETS_H__

#include "Panel.h"
#include <list>
#include <stack>

class PanelAssets : public Panel
{
public:
	PanelAssets(const char* name);
	~PanelAssets();

	void Draw();

	void pushElement(assetsElement* element);

	void clearElements();

public:

	std::list<assetsElement*> elements;
	assetsElement* currElement = nullptr;

	std::stack<assetsElement*> elementsHistory;
};

#endif // !__PANELASSETS_H__