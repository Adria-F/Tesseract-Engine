#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleResource.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleGUI.h"
#include "ModuleTextures.h"
#include "ModuleMeshes.h"
#include "ModuleFileSystem.h"
#include "ModuleSceneLoader.h"

#include "JSONManager.h"

using namespace std;

Application::Application()
{
	JSON_manager = new JSONManager();
	
	window = new ModuleWindow();
	input = new ModuleInput();

	resources = new ModuleResource();
	scene_intro = new ModuleScene();

	renderer3D = new ModuleRenderer3D();
	camera = new ModuleCamera3D();
	gui = new ModuleGUI();	
	scene_loader = new ModuleSceneLoader();
	textures = new ModuleTextures();
	meshes = new ModuleMeshes();
	fileSystem = new ModuleFileSystem();

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(fileSystem);
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(scene_loader);
	AddModule(textures);
	AddModule(meshes);

	// Scenes
	AddModule(resources);
	AddModule(scene_intro);
	
	// Gui
	AddModule(gui);

	// Renderer last!
	AddModule(renderer3D);
}

Application::~Application()
{
	for (list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend() && (*item)==NULL; item++)
	{
		RELEASE((*item));
	}
}

bool Application::Init()
{
	bool ret = true;

	std::string path = SETTINGS_FOLDER;
	path += "config.json";
	JSON_File* document = JSON_manager->openReadFile(path.c_str());

	if (document == nullptr)
	{
		SaveDefaultConfig(path.c_str());
		document = JSON_manager->openReadFile(path.c_str());
	}

	if (document != nullptr)
	{
		//init vars from config
		JSON_Value* appConfig = document->getValue("App");
		if (appConfig != nullptr)
		{
			appName = appConfig->getString("title");
			framerateCap = appConfig->getInt("frameCap");
		}

		//TO be able to use RNG in any file without worrying about initializing the seed
		START_RNG_SEED();

		// Call Init() in all modules
		for (list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret == true; item++)
		{
			ret = (*item)->Init(document);
		}
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");

	for (list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret==true; item++)
	{
		ret = (*item)->Start();
	}
	
	JSON_manager->closeFile(document);
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.ReadTime() / 1000.0f;

	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	int ms_cap = 1000 / framerateCap;
	if (ms_timer.ReadTime() < ms_cap)
		SDL_Delay(ms_cap - ms_timer.ReadTime());

	gui->logFPS(1 / dt, dt * 1000);

	if (doSave)
	{
		scene_loader->saveScene(scene_loader->next_scene_path.c_str());
		doSave = false;
	}
	if (doLoad)
	{
		scene_loader->loadScene(scene_loader->next_scene_path.c_str());
		doLoad = false;
	}
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();

	for (list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret == UPDATE_CONTINUE; item++)
	{
		ret = (*item)->PreUpdate(dt);
	}

	for (list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret == UPDATE_CONTINUE; item++)
	{
		ret = (*item)->Update(dt);
	}

	for (list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret == UPDATE_CONTINUE; item++)
	{
		ret = (*item)->PostUpdate(dt);
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend() && ret == true; item++)
	{
		ret = (*item)->CleanUp();
	}

	return ret;
}

const char* Application::getAppName() const
{
	return appName.c_str();
}

void Application::setAppName(const char * name)
{
	if (name != nullptr && name != appName)
	{
		appName = name;
		SDL_SetWindowTitle(window->window, name);
	}
}

int Application::getFramerateCap() const
{
	return framerateCap;
}

void Application::setFramerateCap(int cap)
{
	framerateCap = cap;
}

void Application::RequestBrowser(const char * url) const
{
	ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

bool Application::SaveDefaultConfig(const char* path)const
{
	bool ret = true;

	JSON_File* document = JSON_manager->openWriteFile(path);

	JSON_Value* app = document->createValue();
	app->addString("title", "Tesseract-Engine");
	app->addInt("frameCap", 60);
	document->addValue("App", app);

	for (list<Module*>::const_iterator item = list_modules.begin(); item != list_modules.end() && ret == true; item++)
	{
		ret = (*item)->SaveDefaultConfig(document);
	}

	document->Write();
	
	JSON_manager->closeFile(document);
	
	return ret;
}

void Application::Load()
{
	doLoad = true;
}

void Application::Save()
{
	doSave = true;
}

float Application::GetDeltaTime() const
{
	return dt;
}
