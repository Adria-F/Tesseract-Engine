#ifndef __PANEL_H__
#define __PANEL_H__

#include "Globals.h"
#include "Module.h"
#include "ImGui\imgui.h"
#include <string>
#include <list>

struct assetsElement
{
	~assetsElement()
	{
		for (std::list<assetsElement*>::iterator it_e = elements.begin(); it_e != elements.end(); it_e++)
		{
			RELEASE(*it_e);
		}
		elements.clear();
	}

	enum elementType
	{
		FOLDER = 0,
		FILE
	}type;

	std::string name;
	std::list<assetsElement*> elements;

	void pushElement(assetsElement* element)
	{
		elements.push_back(element);
	}
};

class Panel
{
public:
	Panel(const char* name) : name(name) {}
	virtual ~Panel() {}

	virtual void Draw() {}
	void toggleActive() { active = !active; }

	bool isActive() const { return active; }
	const char* getName() const { return name.c_str(); }

protected:

	std::string name;
	bool active = false;

};

#endif // !__PANEL_H__