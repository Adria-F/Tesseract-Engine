#ifndef __MODULE_CAMERA_3D_H__
#define __MODULE_CAMERA_3D_H__

#include "Module.h"

#define CAMERA_SPEED 10.0f

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Init(JSON_File* document);
	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	void FitCamera(const AABB &boundingBox);
	float* GetViewMatrix();
	vec3 getMovementFactor();

	bool Save(JSON_File* document)const;
	bool Load(JSON_File* document);

private:

	void CalculateViewMatrix();

public:
	
	vec3 X = { 1.0f,0.0f,0.0f };
	vec3 Y = { 0.0f,1.0f,0.0f };
	vec3 Z = { 0.0f,0.0f,1.0f };
	vec3 Position = { 0.0f,0.0f,10.0f }; 
	vec3 Reference = { 0.0f,0.0f,0.0f };

	float cameraSpeed = 0;
	float mouseSensitivity = 0;
	float wheelSensitivity = 0;
	float zoomDistance = 0;
	AABB* BBtoLook=nullptr;

private:

	mat4x4 ViewMatrix;
};

#endif //__MODULE_CAMERA_3D_H__