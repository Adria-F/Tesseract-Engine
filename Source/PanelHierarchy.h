#ifndef __PANELHIERARCHY_H__ 
#define __PANELHIERARCHY_H__


#include "Panel.h"

class GameObject;

class PanelHierarchy : public Panel
{
public:
	PanelHierarchy(const char* name);
	~PanelHierarchy();

	void Draw();
	void FillTree(GameObject* gameobject);

public: 

	bool creating = false;

};

#endif //__PANELHIERARCHY_H__