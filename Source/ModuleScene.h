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
class ComponentCamera;
class Quadtree;

class ModuleScene : public Module
{
public:
	ModuleScene(bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	bool Save(JSON_File* document);

	bool Load(JSON_File* document);

	void Draw();

	void wantToSaveScene();
	void wantToLoadScene();

	void newScene();

	void FillQuadtree(GameObject* gameObject);

public:

	std::string scene_name = "";

	list<Primitive*> ShapesToDraw;
	vector<GameObject*> GameObjects;
	vector<GameObject*> ObjectsToDraw;

	GameObject* selected_GO = nullptr;
	GameObject* cameraCulling = nullptr;

	ComponentCamera* auxCameraCulling = nullptr;
	Quadtree* quadTree=nullptr;

	GameObject* test_1 = nullptr;
	GameObject* test_2 = nullptr;
	GameObject* test_3 = nullptr;

};
#endif // !__MODULESCENE_H__
