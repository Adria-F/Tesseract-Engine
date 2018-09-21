#ifndef __PANEL_H__
#define __PANEL_H__

#include "Globals.h"
#include <string>

class Panel
{
public:
	Panel(const char* name): name(name){}
	virtual ~Panel() {}

	virtual void Draw() {}
	void toggleActive() { active = !active; }

	bool isActive() const { return active; }

protected:

	std::string name;
	bool active = false;

	public:

	float posX, posY, width, height;

};

#endif // !__PANEL_H__