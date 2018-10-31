#ifndef __MODULESCENE_H__
#define __MODULESCENE_H__

#include "Module.h"
#include "Timer.h"
#include "MathGeoLib/MathGeoLib.h"

#include <list>
#include <vector>

#define ROAD_HEIGHT 9
#define ROAD_WIDTH 15
#define INVISIBLE_WALL 12
#define MAP_SCALE 2

using namespace std;

class Primitive;
class GameObject;

class ModuleScene : public Module
{
public:
	ModuleScene(bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();
	//void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

	bool Save(JSON_File* document);

	bool Load(JSON_File* document);

	void Draw();

	void newScene();

public:

	list<Primitive*> ShapesToDraw;
	vector<GameObject*> GameObjects;

	GameObject* selected_GO = nullptr;

};
#endif // !__MODULESCENE_H__
