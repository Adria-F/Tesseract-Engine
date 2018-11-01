#ifndef __GAMEOBJECTS_H__
#define __GAMEOBJECTS_H__

#include <list>
#include <string>

struct Mesh;

class Component;
class ComponentTransformation;

enum componentType;

class GameObject
{
public:
	GameObject();
	~GameObject();

	Component* AddComponent(componentType type);
	void RemoveComponent(Component* component);

	void Update();
	void Draw();
	void DrawBB(const AABB& BB, vec3 color) const;

	void Save(JSON_Value* gameobject);
	void Load(JSON_Value* gameobject);

public:

	std::list<Component*> components;
	std::vector<GameObject*> childs;

	GameObject* parent = nullptr;

	bool active = true;
	std::string name = "";
	uint parentUID = 0;
	uint UID = 0;

	AABB boundingBox;
};

#endif // !__GAMEOBJECTS_H__