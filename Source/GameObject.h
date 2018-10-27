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
	void RemoveComponent(Component& component);

	ComponentTransformation* CreateComponentTransformation();

	void Update();
	void Draw();
	void DrawBB(const AABB& BB, vec3 color)const;

public:

	std::list<Component*> components;
	std::vector<GameObject*> childs;

	GameObject* parent = nullptr;

	bool active = true;
	std::string name = "";

	AABB boundingBox;
};

#endif // !__GAMEOBJECTS_H__