#pragma once

#include "Panel.h"

class PanelElements:public Panel
{
public:
	PanelElements(const char* name, float posX, float posY, float width, float height);
	~PanelElements();

	void Draw();

private:

	float Box_C[3] = { 0,0,0 };
	float Box_S[3] = { 0,0,0 };

	int S_Ring = 0;
	int S_Sectors = 0;
	float Sphere_R = 0;
	float Sphere_C[3] = { 0,0,0 };

	int C_Ring = 0;
	int C_Sectors = 0;
	float Cylinder_R = 0;
	float Cylinder_H = 0;
	float Cylinder_C[3] = { 0,0,0 };

};

