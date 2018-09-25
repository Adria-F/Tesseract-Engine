#include "Application.h"
#include "ModulePhysics3D.h"

ModulePhysics3D::ModulePhysics3D(bool start_enabled) : Module(start_enabled)
{

}

// Destructor
ModulePhysics3D::~ModulePhysics3D()
{
	
}

// Render not available yet----------------------------------
bool ModulePhysics3D::Init(rapidjson::Document& document)
{
	LOG("Creating 3D Physics simulation");
	bool ret = true;

	return ret;
}

// ---------------------------------------------------------
bool ModulePhysics3D::Start()
{
	LOG("Creating Physics environment");

	//world = new btDiscreteDynamicsWorld(dispatcher, broad_phase, solver, collision_conf);
	//world->setDebugDrawer(debug_draw);
	//world->setGravity(GRAVITY);
	//vehicle_raycaster = new btDefaultVehicleRaycaster(world);

	//// Big plane as ground
	//{
	//	btCollisionShape* colShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);

	//	btDefaultMotionState* myMotionState = new btDefaultMotionState();
	//	btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, myMotionState, colShape);

	//	btRigidBody* body = new btRigidBody(rbInfo);
	//	world->addRigidBody(body);
	//}

	return true;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::PreUpdate(float dt)
{
	/*world->stepSimulation(dt, 15);

	int numManifolds = world->getDispatcher()->getNumManifolds();
	for(int i = 0; i<numManifolds; i++)
	{
		btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
		btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());

		int numContacts = contactManifold->getNumContacts();
		if(numContacts > 0)
		{
			PhysBody3D* pbodyA = (PhysBody3D*)obA->getUserPointer();
			PhysBody3D* pbodyB = (PhysBody3D*)obB->getUserPointer();

			if(pbodyA && pbodyB)
			{
				p2List_item<Module*>* item = pbodyA->collision_listeners.getFirst();
				while(item)
				{
					item->data->OnCollision(pbodyA, pbodyB);
					item = item->next;
				}

				item = pbodyB->collision_listeners.getFirst();
				while(item)
				{
					item->data->OnCollision(pbodyB, pbodyA);
					item = item->next;
				}
			}
		}
	}
*/
	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::Update(float dt)
{
	/*if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(debug == true)
	{
		world->debugDrawWorld();		

		if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		{
			Sphere s(1);
			s.SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);
			float force = 30.0f;
			AddBody(s)->Push(-(App->camera->Z.x * force), -(App->camera->Z.y * force), -(App->camera->Z.z * force));
		}
	}*/

	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModulePhysics3D::CleanUp()
{
	LOG("Destroying 3D Physics simulation");

	//// Remove from the world all collision bodies
	//for(int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
	//{
	//	btCollisionObject* obj = world->getCollisionObjectArray()[i];
	//	world->removeCollisionObject(obj);
	//}

	//for(p2List_item<btTypedConstraint*>* item = constraints.getFirst(); item; item = item->next)
	//{
	//	world->removeConstraint(item->data);
	//	delete item->data;
	//}
	//
	//constraints.clear();

	//for(p2List_item<btDefaultMotionState*>* item = motions.getFirst(); item; item = item->next)
	//	delete item->data;

	//motions.clear();

	//for(p2List_item<btCollisionShape*>* item = shapes.getFirst(); item; item = item->next)
	//	delete item->data;

	//shapes.clear();

	//for(p2List_item<PhysBody3D*>* item = bodies.getFirst(); item; item = item->next)
	//	delete item->data;

	//bodies.clear();

	//delete vehicle_raycaster;
	//delete world;

	return true;
}

Sphere* ModulePhysics3D::CreateSphere(float3 position, float radius)
{
	Sphere* aux_sphere=new Sphere(position,radius);

	return aux_sphere;
}

OBB* ModulePhysics3D::CreateOBB(float3 position, float3 size, float3 vec_1, float3 vec_2, float3 vec_3)
{
	OBB* aux_cube = new OBB(position, size,vec_1,vec_2,vec_3);

	return aux_cube;
}

Capsule* ModulePhysics3D::AddCapsule(LineSegment height, float radius)
{
	Capsule* aux_capsule;

	return aux_capsule;
}

// ---------------------------------------------------------
//PhysBody3D* ModulePhysics3D::AddBody(const Sphere& sphere, float mass)
//{
//	btCollisionShape* colShape = new btSphereShape(sphere.radius);
//	shapes.add(colShape);
//
//	btTransform startTransform;
//	startTransform.setFromOpenGLMatrix(&sphere.transform);
//
//	btVector3 localInertia(0, 0, 0);
//	if(mass != 0.f)
//		colShape->calculateLocalInertia(mass, localInertia);
//
//	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
//	motions.add(myMotionState);
//	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
//
//	btRigidBody* body = new btRigidBody(rbInfo);
//	PhysBody3D* pbody = new PhysBody3D(body);
//
//	body->setUserPointer(pbody);
//	world->addRigidBody(body);
//	bodies.add(pbody);
//
//	return pbody;
//}


// ---------------------------------------------------------
//PhysBody3D* ModulePhysics3D::AddBody(const Cube& cube, float mass)
//{
//	btCollisionShape* colShape = new btBoxShape(btVector3(cube.size.x*0.5f, cube.size.y*0.5f, cube.size.z*0.5f));
//	shapes.add(colShape);
//
//	btTransform startTransform;
//	startTransform.setFromOpenGLMatrix(&cube.transform);
//
//	btVector3 localInertia(0, 0, 0);
//	if(mass != 0.f)
//		colShape->calculateLocalInertia(mass, localInertia);
//
//	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
//	motions.add(myMotionState);
//	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
//
//	btRigidBody* body = new btRigidBody(rbInfo);
//	PhysBody3D* pbody = new PhysBody3D(body);
//
//	body->setUserPointer(pbody);
//	world->addRigidBody(body);
//	bodies.add(pbody);
//
//	return pbody;
//}

// ---------------------------------------------------------
//PhysBody3D* ModulePhysics3D::AddBody(const Cylinder& cylinder, float mass)
//{
//	btCollisionShape* colShape = new btCylinderShapeX(btVector3(cylinder.height*0.5f, cylinder.radius, 0.0f));
//	shapes.add(colShape);
//
//	btTransform startTransform;
//	startTransform.setFromOpenGLMatrix(&cylinder.transform);
//
//	btVector3 localInertia(0, 0, 0);
//	if(mass != 0.f)
//		colShape->calculateLocalInertia(mass, localInertia);
//
//	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
//	motions.add(myMotionState);
//	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
//
//	btRigidBody* body = new btRigidBody(rbInfo);
//	PhysBody3D* pbody = new PhysBody3D(body);
//
//	body->setUserPointer(pbody);
//	world->addRigidBody(body);
//	bodies.add(pbody);
//
//	return pbody;
//}

// ---------------------------------------------------------



bool ModulePhysics3D::Save(rapidjson::Document& document) {
	return true;
}
bool ModulePhysics3D::Load(rapidjson::Document& document) {
	return true;
}