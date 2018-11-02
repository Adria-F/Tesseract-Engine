#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Globals.h"
#include "Timer.h"

#include "MathGeoLib/MathGeoLib.h"
#include "glMath/glmath.h"
#include "ImGui\imgui.h"

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
class ModuleFileSystem;
class ModuleSceneLoader;

class Application
{
public:
	JSONManager* JSON_manager = nullptr;
	
	ModuleWindow* window=nullptr;
	ModuleInput* input=nullptr;
	ModuleScene* scene_intro=nullptr;
	ModuleRenderer3D* renderer3D = nullptr;
	ModuleCamera3D* camera = nullptr;
	ModuleGUI* gui = nullptr;
	ModuleTextures* textures = nullptr;
	ModuleMeshes* meshes = nullptr;
	ModuleFileSystem* fileSystem = nullptr;
	ModuleSceneLoader* scene_loader= nullptr;

private:

	Timer	ms_timer;
	float	dt;

	std::list<Module*> list_modules;
	std::string appName = "";

	int framerateCap = 60;

	bool doSave = false;
	bool doLoad = false;

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

	void RequestBrowser(const char * url) const;

	bool LoadConfig();
	bool SaveConfig() const;

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