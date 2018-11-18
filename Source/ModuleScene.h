#ifndef __MODULESCENE_H__
#define __MODULESCENE_H__

#include "Module.h"
#include "Timer.h"
#include "MathGeoLib/MathGeoLib.h"

#include <list>
#include <vector>
#include <map>

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

	void StartQuadTree();
	void FillQuadtree(GameObject* gameObject); 
	void ResizeQuadTree(GameObject* gameObject);
	void DrawGuizmo(ImGuizmo::OPERATION operation);

	void addGameObject(GameObject* gameObject, GameObject* parent = nullptr);
	void AddCamera();
	void AddEmptyGameObject();
	void deleteGameObject(GameObject* GO);
	void selectGameObject(GameObject* gameObject);
	GameObject* getGameObject(uint UID);

	//Provisional
	void FindCameras(GameObject* parent);
	void ChangeCulling(GameObject* GO);

public:

	std::string scene_name = "";
	std::vector<GameObject*> cameras;

	list<Primitive*> ShapesToDraw;

	GameObject* root = nullptr;

	list<GameObject*> ObjectsToDraw;

	GameObject* selected_GO = nullptr;

	Quadtree* quadTree=nullptr;

	ImGuizmo::OPERATION guizmoOperation;

private:

	map<uint, GameObject*> gameObjects;
};
#endif // !__MODULESCENE_H__
