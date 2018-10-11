#ifndef __MODULESCENE_H__
#define __MODULESCENE_H__

#include "Module.h"
#include "Timer.h"
#include "MathGeoLib/MathGeoLib.h"

#include <list>

#define ROAD_HEIGHT 9
#define ROAD_WIDTH 15
#define INVISIBLE_WALL 12
#define MAP_SCALE 2

using namespace std;

class Primitive;

class ModuleScene : public Module
{
public:
	ModuleScene(bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();
	//void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

	bool Save(rapidjson::Document& document, rapidjson::FileWriteStream& os);

	bool Load(rapidjson::Document& document);

	void Draw();

	void newScene();

public:

	Sphere S_Test_A;
	Sphere S_Test_B;
	AABB   AABB_Test_A;
	AABB   AABB_Test_B;

	list<Primitive*> ShapesToDraw;

};
#endif // !__MODULESCENE_H__
