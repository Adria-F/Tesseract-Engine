#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "p2List.h"
#include "Globals.h"
#include "Timer.h"

#include "MathGeoLib/MathGeoLib.h"
#include "ImGui\imgui.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filewritestream.h"
#include "Glew/include/glew.h"
#include "SDL\include\SDL_opengl.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#include <list>
#include <string>

class Module;
class ModuleWindow;
class ModuleInput;
class ModuleAudio;
class ModuleSceneIntro;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModulePhysics3D;
class ModuleGUI;

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleSceneIntro* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModulePhysics3D* physics;
	ModuleGUI* gui;

private:

	Timer	ms_timer;
	float	dt;
	//p2List<Module*> list_modules;
	std::list<Module*> list_modules;

	int framerateCap = 60;
	std::string appName = TITLE;

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

	bool LoadGame();
	bool SaveGame();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

};

extern Application* App;

#endif //__APPLICATION_H__