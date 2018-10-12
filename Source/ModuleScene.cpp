#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Primitive.h"
#include "ModuleRenderer3D.h"
#include "ModuleMeshLoader.h"

ModuleScene::ModuleScene(bool start_enabled) : Module(start_enabled)
{}

ModuleScene::~ModuleScene()
{}

bool ModuleScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	//Load Baker House model
	App->mesh_loader->ImportFBX("Assets/Models/BakerHouse.fbx");

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

bool ModuleScene::CleanUp()
{
	LOG("Unloading Intro scene");
	return true;
}

update_status ModuleScene::Update(float dt)
{
	update_status ret = UPDATE_CONTINUE;

	return ret;
}

bool ModuleScene::Save(rapidjson::Document& document, rapidjson::FileWriteStream& os) {

	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
	document.AddMember("name", "Scene", allocator);
	rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
	return true;
}
bool ModuleScene::Load(rapidjson::Document& document) {
	return true;
}

void ModuleScene::Draw()
{
	for (list<Primitive*>::iterator it = ShapesToDraw.begin(); it != ShapesToDraw.end(); it++)
	{
		(*it)->Render();
	}
}

void ModuleScene::newScene()
{
	std::list<Primitive*>::iterator it_p;
	it_p = ShapesToDraw.begin();
	while (it_p != ShapesToDraw.end())
	{
		delete ((*it_p));
		(*it_p) = nullptr;
		it_p++;
	}
	ShapesToDraw.clear();

	std::list<Mesh*>::iterator it_m;
	it_m = App->renderer3D->meshes.begin();
	while (it_m != App->renderer3D->meshes.end())
	{
		delete ((*it_m));
		(*it_m) = nullptr;
		it_m++;
	}
	App->renderer3D->meshes.clear();
}
