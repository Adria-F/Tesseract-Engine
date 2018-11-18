#ifndef __PANELCINFIGURATION_H__
#define __PANELCINFIGURATION_H__

#include "Panel.h"
#include <vector>

#define FPS_LOG_SIZE 100
#define MEMORY_LOG_SIZE 100

class Module;
class Application;

class PanelConfiguration : public Panel
{
public:
	PanelConfiguration(const char* name);
	~PanelConfiguration();

	void Draw();

	void addFPS(float fps, float ms);
	void addGameFPS(float fps, float ms);
	void addMemory(float memory);

public:

	std::vector<float> fps_log;
	std::vector<float> game_fps_log;
	std::vector<float> ms_log;
	std::vector<float> game_ms_log;
	std::vector<float> memory_log;

private:

	bool fullscreen = false;
	bool resizable = false;
	bool borderless = false;
	bool full_desktop = false;
	
	float brightness=0.0f;
	
	SDL_DisplayMode mode;

	int prevWidth=0;
	int prevHeight=0;

};

#endif // !__PANELCINFIGURATION_H__