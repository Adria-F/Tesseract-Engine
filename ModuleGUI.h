#ifndef __MODULEGUI_H__
#define __MODULEGUI_H__

#include "Module.h"

class ModuleGUI : public Module
{
public:
	ModuleGUI(Application* app, bool start_enabled = true);
	~ModuleGUI();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

public:

	bool demoWindow = false;
	bool Sphere_tb = false;
	bool RNGb = false;
	
	float aux_A[4] = { 0.0f,0.0f,0.0f,0.0f };
	float aux_B[4] = { 0.0f,0.0f,0.0f,0.0f };

	float firstR = 0.0f;
	int secondR = 0;
	int min = -50;
	int max = 50;
};

#endif // !__MODULEGUI_H__