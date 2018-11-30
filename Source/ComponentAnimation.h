#ifndef __COMPONENTANIMATION_H__
#define __COMPONENTANIMATION_H__

#include "Component.h"

class GameObject;

class ComponentAnimation : public Component
{
public:
	ComponentAnimation(GameObject* parent, componentType type);
	~ComponentAnimation();
};

#endif // !__COMPONENTANIMATION_H__