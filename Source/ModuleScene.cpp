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
	StartQuadTree();

	//Load Baker House model
	App->scene_loader->importFBXScene("Assets/Models/BakerHouse.fbx");
	//App->scene_loader->loadScene("sceneTest");
	

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
		StartQuadTree();
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

	ComponentCamera* activeCamera = App->camera->camera;
	//Static objects-------------------------------------------------------------------
	if (quadTree->QT_Box != nullptr)
	{
		//Fill the vector of the objects inside the same quads of the camera's bb		
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


	if (App->scene_intro->selected_GO != nullptr )
	{
		if(App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_IDLE)
		{
			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			{
				guizmoOperation = ImGuizmo::TRANSLATE;
			}
			if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
			{
				guizmoOperation = ImGuizmo::ROTATE;
			}
			if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT)
			{
				guizmoOperation = ImGuizmo::SCALE;
			}
		}
	}

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
	quadTree->QT_Box = new AABB();
	quadTree->QT_Box->SetNegativeInfinity();

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
		quadTree->QT_Box->Enclose(gameObject->boundingBox);

		for (int i = 0; i < gameObject->childs.size(); i++)
		{
			ResizeQuadTree(gameObject->childs[i]);
		}
	}

}

void ModuleScene::DrawGuizmo(ImGuizmo::OPERATION operation)
{
	ComponentTransformation* transform = (ComponentTransformation*)App->scene_intro->selected_GO->GetComponent(TRANSFORMATION);

	if (transform != nullptr)
	{
		ImGuizmo::Enable(true);
		ImVec2 cursorPos = { App->gui->sceneX,App->gui->sceneY };
		ImVec2 windowSize = { App->gui->sceneW,App->gui->sceneH };
		ImGuizmo::SetRect(cursorPos.x, cursorPos.y, windowSize.x, windowSize.y);

		float4x4 ViewMatrix, ProjectionMatrix;
		ImGuizmo::MODE mode;
		float4x4* ObjectMat;

		glGetFloatv(GL_MODELVIEW_MATRIX, (float*)ViewMatrix.v);
		glGetFloatv(GL_PROJECTION_MATRIX, (float*)ProjectionMatrix.v);
		ObjectMat = &transform->localMatrix;
		ObjectMat->Transpose();

		ImGuizmo::SetOrthographic(false);

		

		ImGuizmo::Manipulate((float*)ViewMatrix.v, (float*)ProjectionMatrix.v, operation, ImGuizmo::LOCAL, (float*)ObjectMat,NULL,NULL);
		ObjectMat->Transpose();

		if (ImGuizmo::IsUsing())
		{
			App->renderer3D->CalculateGlobalMatrix(App->scene_intro->GameObjects[0]);
			GameObjects[0]->RecalculateBB();

			StartQuadTree();
		}
	}
}

void ModuleScene::selectGameObject(GameObject* gameObject)
{
	if (selected_GO != nullptr)
		selected_GO->setSelected(false);

	selected_GO = gameObject;
	if (gameObject != nullptr)
		gameObject->setSelected(true);
}
