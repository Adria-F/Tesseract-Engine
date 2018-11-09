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
class PanelInspector;
class PanelScene;
class PanelAssets;
class PanelHierarchy;
class PanelFileDialog;

class Texture;

class ModuleGUI : public Module
{
public:
	ModuleGUI(bool start_enabled = true);
	~ModuleGUI();

	bool Init(JSON_File* document);
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void logFPS(float fps, float ms);
	void logMemory(float memory);
	void AddLog(const char* log);
	void handleInput(SDL_Event* event);

	void SaveDialogAt(const char* path);
	void LoadDialogAt(const char* path);

	void saveDefaultImgui();

	void Draw();

	bool isMouseOnGUI() const;
	bool GUIKeppsKeyboard() const;

public:

	std::list<Panel*> panels;

	PanelConfiguration* configuration = nullptr;
	PanelInspector* inspector = nullptr;
	PanelConsole* console = nullptr;
	PanelAbout* about = nullptr;
	PanelHardwareInfo* hardwareInfo = nullptr;
	PanelElements* ShapeElements = nullptr;
	PanelScene* Scene = nullptr;
	PanelAssets* assets = nullptr;
	PanelHierarchy* hierarchy = nullptr;
	PanelFileDialog* fileDialog = nullptr;

	Texture* folder = nullptr;
	Texture* file = nullptr;

	float sceneX = 0.0f;
	float sceneY = 0.0f;
	float sceneW = 0.0f;
	float sceneH = 0.0f;

	bool hoveringScene = false;
};

#endif // !__MODULEGUI_H__