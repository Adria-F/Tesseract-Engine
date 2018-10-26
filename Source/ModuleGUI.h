#ifndef __MODULEGUI_H__
#define __MODULEGUI_H__

#include "Module.h"

#include <list>

union SDL_Event;

class Panel;
class PanelConfiguration;
class PanelConsole;
class PanelAbout;
class PanelHardwareInfo;
class PanelElements;
class PanelProperties;
class PanelScene;
class PanelAssets;

class Texture;

class ModuleGUI : public Module
{
public:
	ModuleGUI(bool start_enabled = true);
	~ModuleGUI();

	bool Init(rapidjson::Document& document);
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void logFPS(float fps, float ms);
	void logMemory(float memory);
	void AddLog(const char* log);
	void handleInput(SDL_Event* event);

	bool Save(rapidjson::Document& document,rapidjson::FileWriteStream& os)const;
	bool Load(rapidjson::Document& document);

	void Draw();

	bool isMouseOnGUI() const;

public:

	std::list<Panel*> panels;

	PanelConfiguration* configuration = nullptr;
	PanelProperties* properties = nullptr;
	PanelConsole* console = nullptr;
	PanelAbout* about = nullptr;
	PanelHardwareInfo* hardwareInfo = nullptr;
	PanelElements* ShapeElements = nullptr;
	PanelScene* Scene = nullptr;
	PanelAssets* assets = nullptr;

	Texture* folder = nullptr;
	Texture* file = nullptr;

	bool hoveringScene = false;
};

#endif // !__MODULEGUI_H__