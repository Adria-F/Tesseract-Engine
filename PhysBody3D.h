#ifndef __PhysBody3D_H__
#define __PhysBody3D_H__

#include "p2List.h"
#include "p2SString.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class btRigidBody;
class Module;
class btQuaternion;

// =================================================
struct PhysBody3D
{
	friend class ModulePhysics3D;
public:
	PhysBody3D(btRigidBody* body);
	~PhysBody3D();

	void Push(float x, float y, float z);
	void GetTransform(float* matrix) const;
	void SetTransform(const float* matrix) const;
	void SetPos(float x, float y, float z);
	btVector3 GetPos() const;
	btQuaternion GetRotation() const;
	btRigidBody* getBody() const;
	void SetRotation(btQuaternion newQ);
	void SetAsSensor(bool is_sensor);
	bool IsSensor() const;

private:
	btRigidBody* body = nullptr;
	bool is_sensor = false;

public:
	p2List<Module*> collision_listeners;
	p2SString name;
	btVector3 forward;
};

#endif // __PhysBody3D_H__