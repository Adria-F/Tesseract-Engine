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
	void AddLog(const char* log);
	void handleInput(SDL_Event* event);

	bool Save(rapidjson::Document& document,rapidjson::FileWriteStream& os);
	bool Load(rapidjson::Document& document);

public:

	std::list<Panel*> panels;

	PanelConfiguration* configuration = nullptr;
	PanelConsole* console = nullptr;
	PanelAbout* about = nullptr;
	PanelHardwareInfo* hardwareInfo = nullptr;

	// TO DELETE --------------------------------
	bool Sphere_tb = false;
	bool AABB_tb = false;
	bool RNGb = false;
	
	float aux_A[4] = { 0.0f,0.0f,0.0f,1.0f };
	float aux_B[4] = { 0.0f,0.0f,0.0f,1.0f };

	float Max_aux_A[3] = { 1.0f,1.0f,1.0f };
	float Min_aux_A[3] = { 0.0f,0.0f,0.0f };
	float Max_aux_B[3] = { 1.0f,1.0f,1.0f };
	float Min_aux_B[3] = { 0.0f,0.0f,0.0f };

	float firstR = 0.0f;
	int secondR = 0;
	int min = -50;
	int max = 50;
	// -----------------------------------------
};

#endif // !__MODULEGUI_H__