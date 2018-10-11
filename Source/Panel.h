#ifndef __PANEL_H__
#define __PANEL_H__

#include "Globals.h"
#include "Module.h"
#include "ImGui\imgui.h"
#include <string>

enum panelAlingnment
{
	NO_ALINGNMENT = 0,
	CENTER,
	LEFT,
	TOP,
	RIGHT,
	BOTTOM
};

class Panel
{
public:
	Panel(const char* name, float posX, float posY, float width, float height, panelAlingnment aligned = NO_ALINGNMENT) : name(name), pos({ posX, posY }), size({ width, height }), aligned(aligned) {}
	virtual ~Panel() {}

	virtual void Draw() {}
	void toggleActive() { active = !active; }

	bool isActive() const { return active; }
	const char* getName() const { return name.c_str(); }

	void checkMoved()
	{
		if (ImGui::IsMouseReleased(0) && ImGui::IsMouseHoveringWindow())
		{
			if (ImGui::GetWindowPos() != pos)
				pos = ImGui::GetWindowPos();
			if (ImGui::GetWindowSize() != size)
				size = ImGui::GetWindowSize();
		}
	}

protected:

	std::string name;
	bool active = false;

public:

	ImVec2 pos, size;
	panelAlingnment aligned = NO_ALINGNMENT;

};

#endif // !__PANEL_H__