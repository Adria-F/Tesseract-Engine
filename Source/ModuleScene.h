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
class Component;
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

	void FillDrawBuffer(bool game = false);

	void wantToSaveScene();
	void wantToLoadScene();

	void newScene();

	void StartQuadTree();
	void FillQuadtree(GameObject* gameObject); 
	void ResizeQuadTree(GameObject* gameObject);
	void DrawGuizmo(ImGuizmo::OPERATION operation);

	void addGameObject(GameObject* gameObject, GameObject* parent = nullptr, GameObject* fakeScene = nullptr);
	void AddCamera();
	void AddEmptyGameObject();
	void deleteGameObject(GameObject* GO, bool deleteFromParent = true);
	void selectGameObject(GameObject* gameObject);
	GameObject* getGameObject(uint UID);

	void addComponent(Component* component);
	Component* getComponent(uint UID);
	void removeComponent(Component* component);

	//Provisional
	void FindCameras(GameObject* parent);
	void ChangeCulling(GameObject* GO, bool culling = true);

public:

	std::string scene_name = "";
	std::list<GameObject*> cameras;

	GameObject* root = nullptr;
	JSON_File* virtualFile = nullptr;

	list<GameObject*> ObjectsToDraw;

	GameObject* selected_GO = nullptr;
	GameObject* activeCamera = nullptr;

	Quadtree* quadTree=nullptr;

	ImGuizmo::OPERATION guizmoOperation=ImGuizmo::NO_OPERATION;

private:

	//TEST to change animations
	uint idleAnim = 0;
	uint walkAnim = 0;
	uint attackAnim = 0;

	map<uint, GameObject*> gameObjects;
	map<uint, Component*> components;
};
#endif // !__MODULESCENE_H__
