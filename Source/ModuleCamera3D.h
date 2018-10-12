#ifndef __MODULE_CAMERA_3D_H__
#define __MODULE_CAMERA_3D_H__

#include "Module.h"

#define CAMERA_SPEED 10.0f

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Init(rapidjson::Document& document);
	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	void FitCamera(const AABB &boundingBox);
	float* GetViewMatrix();
	vec3 getMovementFactor();

	bool Save(rapidjson::Document& document, rapidjson::FileWriteStream& os)const;
	bool Load(rapidjson::Document& document);

private:

	void CalculateViewMatrix();

public:
	
	vec3 X, Y, Z, Position, Reference;
	float cameraSpeed = 0;
	float mouseSensitivity = 0;
	float wheelSensitivity = 0;
	float zoomDistance = 0;
	AABB* BBtoLook=nullptr;

private:

	mat4x4 ViewMatrix;
};

#endif //__MODULE_CAMERA_3D_H__