#pragma once
#include "Module.h"
#include "p2List.h"

// Recommended scale is 1.0f == 1 meter, no less than 0.2 objects
#define GRAVITY btVector3(0.0f, -10.0f, 0.0f) 

class ModulePhysics3D : public Module
{
public:
	ModulePhysics3D(bool start_enabled = true);
	~ModulePhysics3D();

	bool Init(rapidjson::Document& document);
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	Sphere* CreateSphere(float3 position, float radius = 1.0f);
	OBB* CreateOBB(float3 position, float3 size, float3 vec_1, float3 vec_2, float3 vec_3);
	Capsule* AddCapsule(LineSegment height, float radius = 1.0f);

	

private:
	
	/*p2List<btCollisionShape*> shapes;
	p2List<PhysBody3D*> bodies;
	p2List<btDefaultMotionState*> motions;
	p2List<btTypedConstraint*> constraints;*/
};
