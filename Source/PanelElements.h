#ifndef __PANELELEMENTS_H__
#define __PANELELEMENTS_H__
#include "Panel.h"

class PanelElements:public Panel
{
public:
	PanelElements(const char* name);
	~PanelElements();

	void Draw();

private:

	float Box_C[3] = { 0,0,0 };
	float Box_S[3] = { 0,0,0 };

	int S_Ring = 4;
	int S_Sectors = 4;
	float Sphere_R = 1;
	float Sphere_C[3] = { 0,0,0 };

	int Cy_Ring = 2;
	int Cy_Sectors = 10;
	float Cylinder_R = 1;
	float Cylinder_H = 1;
	float Cylinder_C[3] = { 0,0,0 };

	int Ca_Ring = 0;
	int Ca_Sectors = 0;
	float Capsule_R = 0;
	float Capsule_H = 0;
	float Capsule_C[3] = { 0,0,0 };

	float Frustum_Ds = 0;
	float Frustum_W = 0;
	float Frustum_H = 0;
	float Frustum_D = 0;
	float Frustum_C[3] = { 0,0,0 };

};

#endif //__PANELELEMENTS_H__
