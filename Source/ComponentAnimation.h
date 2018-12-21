#ifndef __COMPONENTANIMATION_H__
#define __COMPONENTANIMATION_H__

#include "Component.h"
#include <map>

class GameObject;

class ComponentAnimation : public Component
{
public:

	ComponentAnimation(GameObject* parent, componentType type);
	~ComponentAnimation();

	bool Update(float dt);
	void DrawInfo();
	void activateDebugBones(GameObject* GO, bool active);

	void assignResource(uint UID);

	void Save(JSON_Value* component) const;
	void Load(JSON_Value* component);

	bool Finished() const;

public:

	bool loop = false;
	bool debugDraw = false;
	bool bonesLoaded = false;

	//bone transformation index and bone GameObject UID
	std::map<uint, uint> bones;

};

#endif // !__COMPONENTANIMATION_H__