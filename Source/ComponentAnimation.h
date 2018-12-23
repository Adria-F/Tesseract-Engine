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

	void assignResource(uint UID, bool doBlend = false,bool blendloop=true);

	void Save(JSON_Value* component) const;
	void Load(JSON_Value* component);

	bool Finished() const;

public:

	bool loop = false;
	bool blendLoop = false;
	bool debugDraw = false;
	bool bonesLoaded = false;
	bool blend = false;
	bool smoothT = false;
	bool frozenT = false;

	uint blendRUID = 0;

	float animTime = 0.0f;
	float blendAnimTime = 0.0f;
	float blendPercent = 0.0f;
	float blendTime = 0.0f;
	float totalBlendTime = 0.1f;
	float speed = 1.0f;

	bool TestPlay = false;

	//bone transformation index and bone GameObject UID
	std::map<uint, uint> bones;
	std::map<uint, uint> blendBones;

};

#endif // !__COMPONENTANIMATION_H__