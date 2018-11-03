#ifndef __MODULE_CAMERA_3D_H__
#define __MODULE_CAMERA_3D_H__

#include "Module.h"

#define CAMERA_SPEED 10.0f

class ComponentCamera;
class GameObject;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Init(JSON_File* document);
	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const vec &Position, const vec &Reference, bool RotateAroundReference = false);
	void LookAt(const vec &Spot);
	void Move(const vec &Movement);
	void FitCamera(const AABB &boundingBox);
	vec getMovementFactor();

	bool Save(JSON_File* document)const;
	bool Load(JSON_File* document);

public:
	
	vec X = { 1.0f,0.0f,0.0f };
	vec Y = { 0.0f,1.0f,0.0f };
	vec Z = { 0.0f,0.0f,1.0f };
	vec Position = { 0.0f,0.0f,10.0f };
	vec Reference = { 0.0f,0.0f,0.0f };

	float cameraSpeed = 0;
	float mouseSensitivity = 0;
	float wheelSensitivity = 0;
	float zoomDistance = 0;
	AABB BBtoLook;

	ComponentCamera* camera = nullptr;

};

#endif //__MODULE_CAMERA_3D_H__