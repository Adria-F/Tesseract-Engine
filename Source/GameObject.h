#ifndef __GAMEOBJECTS_H__
#define __GAMEOBJECTS_H__

#include <list>
#include <string>

class Component;
enum componentType;

class GameObject
{
public:
	GameObject();
	~GameObject();

	Component* AddComponent(Component& component);
	void RemoveComponent(componentType type);

	void Update();

public:

	std::list<Component*> components;
	std::list<GameObject*> childs;

	GameObject* parent = nullptr;

	bool active = true;
	std::string name = "";
};

#endif // !__GAMEOBJECTS_H__