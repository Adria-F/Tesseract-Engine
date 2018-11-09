#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"
#include "JSONManager.h"

class GameObject;
class JSON_Value;

enum componentType
{
	EMPTY_COMPONENT = 0,

	TRANSFORMATION,
	MESH,
	MATERIAL,
	CAMERA
};

class Component
{
public:
	Component(GameObject* gameobject,componentType type):gameObject(gameobject),type(type) { UID = GENERATE_UID(); }
	virtual ~Component() {}

	void DrawUI();
	virtual void DrawInfo() {}
	void DrawConfig();
	virtual bool DrawExtraConfig();
	virtual bool Update() { return true; }

	//Draw a gray button with given string, thought  to be a droppable space for resources inside the components
	void beginDroppableSpace(const char* string, bool empty = false, float2 size = { 250, 20 });
	
	virtual void Enable() { active = true; }
	virtual void Disable() { active = false; }

	virtual void Save(JSON_Value* component) const {}
	virtual void Load(JSON_Value* component) {}

public:

	componentType type = EMPTY_COMPONENT;
	GameObject* gameObject = nullptr;

	bool active = true;
	uint UID = 0;

	bool toDelete = false;
	bool closeConfig = true;
};

#endif // !__COMPONENT_H__