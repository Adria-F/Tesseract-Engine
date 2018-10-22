#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleGUI.h"
#include "ModuleMeshLoader.h"
#include "ModuleFileSystem.h"

#include "rapidjson/filereadstream.h"

using namespace std;
using namespace rapidjson;

Application::Application()
{
	window = new ModuleWindow();
	input = new ModuleInput();
	scene_intro = new ModuleScene();
	renderer3D = new ModuleRenderer3D();
	camera = new ModuleCamera3D();
	gui = new ModuleGUI();
	mesh_loader = new ModuleMeshLoader();
	fileSystem = new ModuleFileSystem();

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(fileSystem);
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(mesh_loader);

	// Scenes
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

	rapidjson::Document document;
	FILE* fp = fopen("config.json", "rb");
	char readBuffer[65536];

	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	document.ParseStream(is);

	//init vars from config

	if (document.HasMember("App"))
	{
		rapidjson::Value& appConfig = document["App"];

		if (appConfig.HasMember("title"))
			appName = document["App"]["title"].GetString();
		if (appConfig.HasMember("frameCap"))
			framerateCap = document["App"]["frameCap"].GetInt();
	}

	//TO be able to use RNG in any file without worrying about initializing the seed
	START_RNG_SEED();

	// Call Init() in all modules
	for (list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret == true; item++)
	{
		ret = (*item)->Init(document);
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");

	for (list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret==true; item++)
	{
		ret = (*item)->Start();
	}
	
	fclose(fp);
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

bool Application::LoadGame()
{
	bool ret=false;

	rapidjson::Document document;
	FILE* fp = fopen("load.json", "rb");
	char readBuffer[65536];

	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	document.ParseStream(is);
	fclose(fp);

	for (list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret == UPDATE_CONTINUE; item++)
	{
		ret = (*item)->Load(document);
	}

	return ret;
}

bool Application::SaveGame()const
{
	bool ret = true;

	rapidjson::Document document;
	document.SetObject();
	FILE* fp = fopen("save.json", "wb");
	char writeBuffer[655360];

	rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

	for (list<Module*>::const_iterator item = list_modules.begin(); item != list_modules.end() && ret == true; item++)
	{
		ret = (*item)->Save(document, os);
	}

	rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
	document.Accept(writer);
	
	fclose(fp);
	
	return ret;
}

float Application::GetDeltaTime() const
{
	return dt;
}
