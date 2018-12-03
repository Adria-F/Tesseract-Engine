#ifndef __GAMEOBJECTS_H__
#define __GAMEOBJECTS_H__

#include <list>
#include <string>

struct Mesh;
struct JSON_Value;

class Component;
class ComponentTransformation;

class ComponentMesh;
class ComponentTexture;
class ComponentAnimation;
class ComponentCamera;

enum componentType;

class GameObject
{
public:
	GameObject();
	~GameObject();

	Component* AddComponent(componentType type);
	void RemoveComponent(Component* component);

	void Update(float dt);
	void DrawComponentsUI();
	void DrawBB(const AABB& BB, vec color) const; 
	void DrawBB(const OBB& BB, vec color) const;

	void Save(JSON_Value* gameobject);
	uint Load(JSON_Value* gameobject);

	Component* GetComponent(componentType type);

	void RecalculateBB();

	void setSelected(bool selected);
	void setChildSelected(bool selected);

	void changeParent(GameObject* newParent, bool recalculateTransformation = true);
	GameObject* getChildByName(const char* name) const;

public:

	std::list<GameObject*> childs;

	GameObject* parent = nullptr;

	bool active = true;
	bool selected = false;
	bool child_selected = false;
	bool isStatic = false;
	bool culling = false;
	bool to_delete = false;
	
	std::string name = "";
	
	uint parentUID = 0;
	uint UID = 0;

	AABB boundingBox;
	OBB localBB;

	ComponentMesh* mesh = nullptr;
	ComponentTexture* texture = nullptr;
	ComponentAnimation* animation = nullptr;
	ComponentCamera* camera = nullptr;
	ComponentTransformation* transformation = nullptr;

private:

	std::list<Component*> components;
};

#endif // !__GAMEOBJECTS_H__