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
#include "ModuleAnimations.h"
#include "ModuleFileSystem.h"
#include "ModuleSceneLoader.h"

#include "JSONManager.h"

#ifdef _DEBUG
	//#define TEST_MEMORY_MANAGER
	#include "mmgr/mmgr.h"
#endif

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
	animations = new ModuleAnimations();
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
	AddModule(animations);

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
	for (list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend() && (*item)!=NULL; item++)
	{
		RELEASE((*item));
	}

	RELEASE(JSON_manager);
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
	//game_timer.Stop();
	GameMode = false;
	counting = false;
	game_dt = 0;
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.ReadTime() / 1000.0f;

	if (!GameMode || GamePaused)
	{
		game_dt = 0;
	}
	else
	{
		game_dt = dt;
	}

	game_dt *= gameSpeed;

	ms_timer.Start();

	/*if (GameMode && !counting)
	{
		counting =true;
		game_timer.Start();
	}*/
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	int ms_cap = 1000 / framerateCap;
	if (ms_timer.ReadTime() < ms_cap)
		SDL_Delay(ms_cap - ms_timer.ReadTime());

	gui->logFPS(1 / dt, dt * 1000);
	
	if (!GameMode || GamePaused)
	{
		gui->logGameFPS(0, 0);
	}
	else
	{
		gameTime += dt / ((float)framerateCap / (float)GameframerateCap);

		gui->logGameFPS(1 / game_dt, game_dt * 1000);
	}
	
	if (doSave)
	{
		if (GameMode)
			scene_loader->saveVirtualScene();
		else
			scene_loader->saveSceneFile(scene_loader->next_scene_path.c_str());
		doSave = false;
	}
	if (doLoad)
	{
		if (GameMode)
		{
			scene_loader->loadVirtualScene();
			App->GameMode = false;
		}
		else
			scene_loader->loadSceneFile(scene_loader->next_scene_path.c_str());
		doLoad = false;
	}

	/*if (!GameMode && counting)
	{
		game_timer.Stop();
		counting = false;
	}*/
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

int Application::getGameFramerateCap() const
{
	return GameframerateCap;
}

void Application::setGameFramerateCap(int cap)
{
	GameframerateCap = cap;
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

void Application::StartGame()
{
	GameMode = true;
}

void Application::PauseGame(bool pause)
{
	if (pause && !GamePaused)
		gamePauseTime = gameTime;
	if (!pause && GamePaused)
		gameTime = gamePauseTime;
	
	GamePaused = pause;
}

void Application::StopGame()
{
	GamePaused = false;
	gameTime = 0;
}

Uint32 Application::ReadGameTime()
{
	if (!GamePaused)
		return gameTime*1000;
	else
		return gamePauseTime*1000;
}

bool Application::inGameMode() const
{
	return GameMode;
}

bool Application::isGamePaused() const
{
	return GamePaused;
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
