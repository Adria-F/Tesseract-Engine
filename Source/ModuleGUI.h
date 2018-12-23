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
class PanelResourcesList;
class PanelGame;
class PanelAnimation;

class Texture;

enum ResType;
class Component;

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
	void logGameFPS(float fps, float ms);
	void logMemory(float memory);
	void AddLog(const char* log);
	void handleInput(SDL_Event* event);

	void SaveDialogAt(const char* path, const char* exclusiveExtension = nullptr);
	void LoadDialogAt(const char* path, const char* exclusiveExtension = nullptr);

	void startResourceList(ResType type, int x, int y, Component* callback, const char* tag);

	void saveDefaultImgui();

	void Draw();

	bool sceneVisible() const;
	bool gameVisible() const;

	//Returns true if scene and game panel are docked into the same element
	//This will cause that just one panel is visible at the same time
	bool sceneAndGameDocked() const;

	void drawToolsMenu();

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
	PanelResourcesList* resourcesList = nullptr;
	PanelGame* GameScene = nullptr;
	PanelAnimation* animations = nullptr;

	Texture* folder = nullptr;
	Texture* file = nullptr;
	Texture* small_folder = nullptr;

	float sceneX = 0.0f;
	float sceneY = 0.0f;
	float sceneW = 0.0f;
	float sceneH = 0.0f;

	bool hoveringScene = false;

	bool lastFocusScene = true;
};

#endif // !__MODULEGUI_H__