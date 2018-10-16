#ifndef __COMPONENT_H__
#define __COMPONENT_H__

class GameObject;

enum componentType
{
	EMPTY_COMPONENT = 0,

	TRANSFORMATION,
	MESH,
	MATERIAL
};

class Component
{
public:
	Component() {}
	~Component() {}

	virtual void DrawInfo() {}
	virtual bool Update() { return true; }
	
	virtual void Enable() { active = true; }
	virtual void Disable() { active = false; }

public:

	componentType type = EMPTY_COMPONENT;
	GameObject* gameObject = nullptr;

	bool active = true;
};

#endif // !__COMPONENT_H__