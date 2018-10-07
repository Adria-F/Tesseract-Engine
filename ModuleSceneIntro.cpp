#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "ModuleRenderer3D.h"

ModuleSceneIntro::ModuleSceneIntro(bool start_enabled) : Module(start_enabled)
{}

ModuleSceneIntro::~ModuleSceneIntro()
{}

bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	//Shapes examples
	/*ShapesToDraw.push_back(new MCube(20, 20, 20, { 25,10,-15 }));
	ShapesToDraw.push_back(new MCylinder(5, 10, 10, 20, {0,0,30}));
	ShapesToDraw.push_back(new MArrow(10, 20, 0, {0,20,0}));
	ShapesToDraw.push_back(new MAxis(5, { 15,20,0 }));
	ShapesToDraw.push_back(new MCapsule(5, 10, 10, 20, { 20,0,30 }));
	ShapesToDraw.push_back(new MSphere(10, 6, 12, { 0.0f, 0.0f, -20.0f }));
	ShapesToDraw.push_back(new MFrustum(10, 10, 5, 3, { -30.0f, 20.0f, 0.0f }));*/

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

	return ret;
}

bool ModuleSceneIntro::Save(rapidjson::Document& document, rapidjson::FileWriteStream& os) {

	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
	document.AddMember("name", "Scene", allocator);
	rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
	return true;
}
bool ModuleSceneIntro::Load(rapidjson::Document& document) {
	return true;
}

void ModuleSceneIntro::Draw()
{
	for (list<Primitive*>::iterator it = ShapesToDraw.begin(); it != ShapesToDraw.end(); it++)
	{
		(*it)->Render();
	}
}

void ModuleSceneIntro::newScene()
{
	ShapesToDraw.clear();
	App->renderer3D->meshes.clear();
}
