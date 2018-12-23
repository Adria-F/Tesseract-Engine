#ifndef __PANELANIMATION_H__
#define __PANELANIMATION_H__

#include "Panel.h"

class ComponentAnimation;
class ResourceAnimation;
class BoneTransform;

class PanelAnimation :
	public Panel
{
public:

	PanelAnimation(const char* name);

	bool fillInfo();

	~PanelAnimation();

	void Draw();

public:

	ComponentAnimation* compAnimation = nullptr;
	ResourceAnimation* animation = nullptr;
	BoneTransform* selectedBone=nullptr;

	float numFrames = 1;
	bool interpolation = false;

private:

	ImVec2 barMovement = { 0, 0 };
	ImVec2 mouseMovement = { 0, 0 };
	
	bool dragging = false;
	bool scrolled = false;
	bool animplay = false;
	bool animpaus = false;

	float recSize=700;
	float zoom = 50;
	float speed = 0.0f;
	float progress = 0.0f;
	float winSize = 0.0f;


};

#endif // !__PANELANIMATION_H__