#ifndef __COMPONENTANIMATION_H__
#define __COMPONENTANIMATION_H__

#include "Component.h"
#include <map>

class GameObject;
class Resource;

class ComponentAnimation : public Component
{
public:

	ComponentAnimation(GameObject* parent, componentType type);
	~ComponentAnimation();

	bool Update(float dt);
	void DrawInfo();
	void activateDebugBones(GameObject* GO, bool active);

	void assignResource(uint UID);
	void assignBlendResource(uint UID);

	void blendAnimation();

	void Save(JSON_Value* component) const;
	void Load(JSON_Value* component);

	bool Finished() const;

public:

	std::vector<Resource*> animations;

	bool loop = false;
	bool debugDraw = false;
	bool bonesLoaded = false;
	bool blend = false;

	uint blendRUID = 0;

	float blendPercent = 0.0f;

	//bone transformation index and bone GameObject UID
	std::map<uint, uint> bones;
	std::map<uint, uint> blendBones;

};

#endif // !__COMPONENTANIMATION_H__