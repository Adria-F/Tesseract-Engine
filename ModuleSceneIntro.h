#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "p2List.h"
#include "PugiXml/src/pugixml.hpp"
#include "Timer.h"
#include "MathGeoLib/MathGeoLib.h"

#define ROAD_HEIGHT 9
#define ROAD_WIDTH 15
#define INVISIBLE_WALL 12
#define MAP_SCALE 2


class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();
	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

public:

	Sphere S_Test_A;
	Sphere S_Test_B;



};
