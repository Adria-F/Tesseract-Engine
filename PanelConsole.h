#ifndef __PANELCONSOLE_H__
#define __PANELCONSOLE_H__

/*
	trying this https://github.com/ocornut/imgui/issues/300
*/

#include "Panel.h"
#include "ImGui\imgui.h"

class PanelConsole : public Panel
{
public:
	PanelConsole(const char* name);
	~PanelConsole();

	void Draw();
	void AddLog(const char* logs);
	void Clear() { Buffer.clear(); }

public:

	ImGuiTextBuffer Buffer;
	bool ScrollToBottom = false;

};

#endif // !__PANELCONSOLE_H__