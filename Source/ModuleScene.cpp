#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Primitive.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleSceneLoader.h"
#include "ModuleGUI.h"
#include "Component.h"
#include "ComponentCamera.h"
#include "ComponentTransformation.h"

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

	//Static objects-------------------------------------------------------------------
	//Fill the vector of the objects inside the same quads of the camera's bb
	ComponentCamera* activeCamera = App->camera->camera;
	quadTree->Intersect(ObjectsToDraw, activeCamera->frustum);

	//From the possible objects only draw the ones inside the frustum
	for (int i = 0; i < ObjectsToDraw.size(); i++)
	{	
		if (activeCamera->ContainsAABB(ObjectsToDraw[i]->boundingBox) && ObjectsToDraw[i]->GetComponent(CAMERA) == nullptr)
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
			if (activeCamera->ContainsAABB(GameObjects[i]->boundingBox) && GameObjects[i]->GetComponent(CAMERA) == nullptr)
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

	DrawGuizmo();

	if (App->renderer3D->ShowQT)
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

void ModuleScene::DrawGuizmo()
{
	if (App->scene_intro->selected_GO != nullptr) 
	{
		float4x4 ViewMatrix, ProjectionMatrix;

		glGetFloatv(GL_MODELVIEW_MATRIX, (float*)ViewMatrix.v);
		glGetFloatv(GL_PROJECTION_MATRIX, (float*)ProjectionMatrix.v);

		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetOrthographic(true);
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

		ComponentTransformation* transform = (ComponentTransformation*)App->scene_intro->selected_GO->GetComponent(TRANSFORMATION);

		ImGuizmo::Manipulate((float*)ViewMatrix.v, (float*)ProjectionMatrix.v, ImGuizmo::TRANSLATE, ImGuizmo::WORLD, (float*)transform->globalMatrix.v);
	}
}
