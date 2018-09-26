#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"

ModuleSceneIntro::ModuleSceneIntro(bool start_enabled) : Module(start_enabled)
{}

ModuleSceneIntro::~ModuleSceneIntro()
{}

bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

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

void ModuleSceneIntro::Draw()const
{
}
