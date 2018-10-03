#ifndef __PANELCINFIGURATION_H__
#define __PANELCINFIGURATION_H__

#include "Panel.h"
#include <vector>

#define FPS_LOG_SIZE 100

class Module;
class Application;

class PanelConfiguration : public Panel
{
public:
	PanelConfiguration(const char* name, float posX, float posY, float width, float height, panelAlingnment aligned = NO_ALINGNMENT);
	~PanelConfiguration();

	void Draw();

	void addFPS(float fps, float ms);
	void windowConfig();

public:

	std::vector<float> fps_log;
	std::vector<float> ms_log;

private:

	bool fullscreen = false;
	bool resizable = false;
	bool borderless = false;
	bool full_desktop = false;

	int nHeight;
	int nWidth;
	
	float brightness;
	
	SDL_DisplayMode mode;

};

#endif // !__PANELCINFIGURATION_H__