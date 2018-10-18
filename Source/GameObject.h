#ifndef __GAMEOBJECTS_H__
#define __GAMEOBJECTS_H__

#include <list>
#include <string>

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

public:

	std::list<Component*> components;
	std::vector<GameObject*> childs;

	GameObject* parent = nullptr;

	bool active = true;
	std::string name = "";
};

#endif // !__GAMEOBJECTS_H__