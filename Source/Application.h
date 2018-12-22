#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Globals.h"
#include "Timer.h"

#include "MathGeoLib/MathGeoLib.h"
#include "ImGui\imgui.h"
#include "ImGuizmo/ImGuizmo.h"

#include "Glew/include/glew.h"
#include "SDL\include\SDL_opengl.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#include <list>
#include <string>

class Module;
class JSONManager;
class ModuleWindow;
class ModuleInput;
class ModuleScene;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleGUI;
class ModuleMeshes;
class ModuleTextures;
class ModuleAnimations;
class ModuleFileSystem;
class ModuleSceneLoader;
class ModuleResource;

class Application
{
public:
	JSONManager* JSON_manager = nullptr;
	
	ModuleWindow* window=nullptr;
	ModuleInput* input=nullptr;
	ModuleResource* resources = nullptr;
	ModuleScene* scene_intro=nullptr;
	ModuleRenderer3D* renderer3D = nullptr;
	ModuleCamera3D* camera = nullptr;
	ModuleGUI* gui = nullptr;
	ModuleTextures* textures = nullptr;
	ModuleMeshes* meshes = nullptr;
	ModuleAnimations* animations = nullptr;
	ModuleFileSystem* fileSystem = nullptr;
	ModuleSceneLoader* scene_loader= nullptr;

	//Timer game_timer;
	float game_dt = 0.0f;
	float gameSpeed = 1.0f;

private:

	Timer	ms_timer;
	float	dt;

	float gameTime = 0;
	float gamePauseTime = 0;
	bool GameMode = false;
	int GameframerateCap = 60;
	bool GamePaused = false;

	std::list<Module*> list_modules;
	std::string appName = "";

	int framerateCap = 60;

	bool doSave = false;
	bool doLoad = false;
	bool counting = false;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	const char* getAppName() const;
	void setAppName(const char* name);

	int getFramerateCap() const;
	void setFramerateCap(int cap);
	int getGameFramerateCap() const;
	void setGameFramerateCap(int cap);

	void RequestBrowser(const char * url) const;

	bool SaveDefaultConfig(const char* path) const;

	void StartGame();
	void PauseGame(bool pause);
	void StopGame();
	//In milliseconds
	Uint32 ReadGameTime();

	bool inGameMode() const;
	bool isGamePaused() const;

	void Load();
	void Save();

	float GetDeltaTime() const;

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

};

extern Application* App;

#endif //__APPLICATION_H__