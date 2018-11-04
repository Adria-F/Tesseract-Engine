#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Primitive.h"
#include "ModuleRenderer3D.h"
#include "ModuleInput.h"
#include "ModuleSceneLoader.h"
#include "ModuleGUI.h"
#include "Component.h"
#include "ComponentCamera.h"

#include "Quadtree.h"

#include "GameObject.h"

ModuleScene::ModuleScene(bool start_enabled) : Module(start_enabled)
{}

ModuleScene::~ModuleScene()
{}

bool ModuleScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	quadTree = new Quadtree();
	//Load Baker House model
	App->scene_loader->importFBXScene("Assets/Models/BakerHouse.fbx");
	//App->scene_loader->loadScene("sceneTest");

	//Shapes examples
	/*ShapesToDraw.push_back(new MCube(20, 20, 20, { 25,10,-15 }));
	ShapesToDraw.push_back(new MCylinder(5, 10, 10, 20, {0,0,30}));
	ShapesToDraw.push_back(new MArrow(10, 10, 10, {0,0,0}));
	ShapesToDraw.push_back(new MAxis(5, { 15,20,0 }));
	ShapesToDraw.push_back(new MCapsule(5, 10, 10, 20, { 20,0,30 }));
	ShapesToDraw.push_back(new MSphere(10, 6, 12, { 0.0f, 0.0f, -20.0f }));
	ShapesToDraw.push_back(new MFrustum(10, 10, 5, 3, { -30.0f, 20.0f, 0.0f }));*/


	//Camera test for the frustum culling

	cameraCulling = new GameObject();
	cameraCulling->name = "Camera";
	auxCameraCulling =(ComponentCamera*)cameraCulling->AddComponent(CAMERA);
	GameObjects.push_back(cameraCulling);


	test_1 = new GameObject();
	test_1->name = "1";
	test_1->boundingBox = AABB({ -2,-2,-9.0f }, { -1,-1,-8 });
	GameObjects.push_back(test_1);

	test_2 = new GameObject();
	test_2->name = "2";
	test_2->boundingBox = AABB({ 2,2,9.0f }, { 1,1,8 });
	GameObjects.push_back(test_2);

	test_3 = new GameObject();
	test_3->name = "3";
	test_3->boundingBox = AABB({ 3,3,5.0f }, { 6,6,6 });
	GameObjects.push_back(test_3);

	
	StartQuadTree();

	return ret;
}

bool ModuleScene::CleanUp()
{
	LOG("Unloading Intro scene");
	newScene();
	return true;
}

update_status ModuleScene::Update(float dt)
{
	update_status ret = UPDATE_CONTINUE;

	vec newPos(0, 0, 0);
	float speed = 10.0f * dt;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed *= 2;
	else if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT)
		speed /= 2;

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) newPos += auxCameraCulling->frustum.front*speed;
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) newPos -= auxCameraCulling->frustum.front*speed;


	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) newPos -= auxCameraCulling->frustum.WorldRight()*speed;
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) newPos += auxCameraCulling->frustum.WorldRight()*speed;

	auxCameraCulling->frustum.pos += newPos;

	if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
	{
		quadTree->Clear();

	}

	if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN)
	{
		quadTree->Clear();

		for (int i = 0; i < GameObjects.size(); i++)
		{
			ResizeQuadTree(GameObjects[i]);
		}

		for (int i = 0; i < GameObjects.size(); i++)
		{
			FillQuadtree(GameObjects[i]);
		}
	}

	return ret;
}

bool ModuleScene::Save(JSON_File* document) {

	JSON_Value* scene = document->createValue();
	scene->addString("name", "scene");
	document->addValue("scene", scene);
	
	return true;
}
bool ModuleScene::Load(JSON_File* document) {
	return true;
}

void ModuleScene::Draw()
{
	for (list<Primitive*>::iterator it = ShapesToDraw.begin(); it != ShapesToDraw.end(); it++)
	{
		(*it)->Render();
	}

	cameraCulling->culling=true;

	//Static objects-------------------------------------------------------------------
	//Fill the vector of the objects inside the same quads of the camera's bb
	quadTree->Intersect(ObjectsToDraw, auxCameraCulling->frustum);

	//From the possible objects only draw the ones inside the frustum
	for (int i = 0; i < ObjectsToDraw.size(); i++)
	{	
		if (auxCameraCulling->ContainsAABB(ObjectsToDraw[i]->boundingBox) && ObjectsToDraw[i]->GetComponent(CAMERA) == nullptr)
		{
			ObjectsToDraw[i]->culling = true;
		}
		else 
		{
			ObjectsToDraw[i]->culling = false;
		}
	}

	//Non-Static objects-------------------------------------------------------------------

	//From the possible objects only draw the ones inside the frustum
	for (int i = 0; i < GameObjects.size(); i++)
	{
		if (!GameObjects[i]->isStatic)
		{
			if (auxCameraCulling->ContainsAABB(GameObjects[i]->boundingBox) && GameObjects[i]->GetComponent(CAMERA) == nullptr)
			{
				GameObjects[i]->culling = true;
			}
			else
			{
				GameObjects[i]->culling = false;
			}
		}
	}

	for (int i = 0; i < GameObjects.size(); i++)
	{
		GameObjects[i]->Update();
		GameObjects[i]->culling = false;
	}

	quadTree->DrawQT();

	ObjectsToDraw.clear();
}

void ModuleScene::wantToSaveScene()
{
	App->gui->SaveDialogAt(SCENES_FOLDER);
}

void ModuleScene::wantToLoadScene()
{
	App->gui->LoadDialogAt(SCENES_FOLDER);
}

void ModuleScene::newScene()
{
	quadTree->Clear();

	selected_GO = nullptr;

	LOG("Unloading scene");
	std::list<Primitive*>::iterator it_p;
	it_p = ShapesToDraw.begin();
	while (it_p != ShapesToDraw.end())
	{
		RELEASE((*it_p));
		it_p++;
	}
	ShapesToDraw.clear();

	std::list<Mesh*>::iterator it_m;
	it_m = App->renderer3D->meshes.begin();
	while (it_m != App->renderer3D->meshes.end())
	{
		RELEASE((*it_m));
		it_m++;
	}
	App->renderer3D->meshes.clear();

	int size = GameObjects.size();
	for (int i = 0; i < size; i++)
	{
		RELEASE(GameObjects[i]);
	}
	GameObjects.clear();
}

void ModuleScene::StartQuadTree()
{
	quadTree->Clear();

	for (int i = 0; i < GameObjects.size(); i++)
	{
		ResizeQuadTree(GameObjects[i]);
	}

	for (int i = 0; i < GameObjects.size(); i++)
	{
		FillQuadtree(GameObjects[i]);
	}
}

void ModuleScene::FillQuadtree(GameObject* gameObject)
{
	if (gameObject != nullptr && gameObject->isStatic)
	{
		quadTree->Insert(gameObject);

		for (int i = 0; i < gameObject->childs.size(); i++)
		{
			FillQuadtree(gameObject->childs[i]);
		}
	}
		
}

void ModuleScene::ResizeQuadTree(GameObject* gameObject)
{
	if (gameObject != nullptr && gameObject->isStatic)
	{
		quadTree->QT_Box.Enclose(gameObject->boundingBox);

		for (int i = 0; i < gameObject->childs.size(); i++)
		{
			ResizeQuadTree(gameObject->childs[i]);
		}
	}

}