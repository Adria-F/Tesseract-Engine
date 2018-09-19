#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"

#include "MathGeoLib/Math/MathAll.h"
#include "MathGeoLib/Geometry/GeometryAll.h"

#include "pcg-cpp-0.98/extras/entropy.h"
#include <time.h>

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{}

ModuleSceneIntro::~ModuleSceneIntro()
{}

bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	pcg32_srandom_r(&rng, time(NULL), (intptr_t)&rng);

	return ret;
}

bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");
	return true;
}

update_status ModuleSceneIntro::Update(float dt)
{
	update_status ret = UPDATE_CONTINUE;

	math::float3 vector_test(32, 5,0.1f);
	math::float3 vector_test_2(29.9f, 5, 0.1f);
	
	math::Sphere esferaesferita(vector_test, 1.0f);
	math::Sphere esferita_2(vector_test_2, 1.0f);

	LOG("x = %f and y = %f and z = %f", vector_test.x, vector_test.y,vector_test.z);
	LOG("x_2 = %f and y_2 = %f and z_2 = %f", vector_test_2.x, vector_test_2.y, vector_test_2.z);

	if (esferita_2.Intersects(esferaesferita))
	{
		LOG("Tete Colisionan!!");
	}
	else
	{
		LOG("EL Lorien es tonto");
	}

	LOG("Random Number: %d", pcg32_random_r(&rng));

	return ret;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	
}
