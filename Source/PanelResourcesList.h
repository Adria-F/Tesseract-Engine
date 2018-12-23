#ifndef __PANELRESOURCESLIST_H__
#define __PANELRESOURCESLIST_H__

#include "Panel.h"
#include "Resource.h"

class Component;

class PanelResourcesList : public Panel
{
public:
	PanelResourcesList(const char* name);
	~PanelResourcesList();

	void Draw();

	void startList(ResType type, int x, int y, Component* callback, const char* tag);

public:

	ResType currentType = ResType::NO_TYPE;
	std::vector<Resource*> resources;
	std::vector<Resource*> extraResources;
	Component* callbackComponent = nullptr;
	std::string tag;

	bool showExtra = false;
};

#endif // !__PANELRESOURCESLIST_H__