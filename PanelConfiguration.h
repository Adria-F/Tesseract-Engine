#ifndef __PANELCINFIGURATION_H__
#define __PANELCINFIGURATION_H__

#include "Panel.h"
#include <vector>

class PanelConfiguration : public Panel
{
public:
	PanelConfiguration(const char* name): Panel(name) {}
	~PanelConfiguration();

	void Draw();

	void addFPS(float fps, float ms);

public:

	std::vector<float> fps_log;
	std::vector<float> ms_log;
};

#endif // !__PANELCINFIGURATION_H__