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
#include "ModuleResource.h"
#include "Resource.h"
#include "ModuleFileSystem.h"

#include "Quadtree.h"

#include "GameObject.h"

#ifdef _DEBUG
//#define TEST_MEMORY_MANAGER
#include "mmgr/mmgr.h"
#endif

ModuleScene::ModuleScene(bool start_enabled) : Module(start_enabled)
{}

ModuleScene::~ModuleScene()
{}

bool ModuleScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	root = new GameObject();
	root->name = "root";
	root->AddComponent(TRANSFORMATION);

	quadTree = new Quadtree();
	StartQuadTree();

	//Load Street scene
	//App->scene_loader->loadScene("Assets/Scenes/sceneTest");
	//App->scene_loader->loadScene("sceneTest");
	
	ImGuizmo::Enable(false);
	
	return ret;
}

bool ModuleScene::CleanUp()
{
	LOG("Unloading Intro scene");
	newScene();

	RELEASE(root);
	RELEASE(quadTree);
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
			if (activeCamera->ContainsAABB(ObjectsToDraw[i]->boundingBox))
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
	for (std::list<GameObject*>::iterator it_ch = root->childs.begin(); it_ch != root->childs.end(); it_ch++)
	{
		if (!(*it_ch)->isStatic)
		{
			if (activeCamera->ContainsAABB((*it_ch)->boundingBox) && (*it_ch)->GetComponent(CAMERA) == nullptr)
			{
				(*it_ch)->culling = true;
			}
			else
			{
				(*it_ch)->culling = false;
			}

			if ((*it_ch)->GetComponent(CAMERA) != nullptr)
			{
				(*it_ch)->culling = true;
			}
		}
	}

	for (std::list<GameObject*>::iterator it_ch = root->childs.begin(); it_ch != root->childs.end(); it_ch++)
	{
		(*it_ch)->Update();
		(*it_ch)->culling = false;
	}


	if (App->scene_intro->selected_GO != nullptr )
	{
		if(App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_IDLE)
		{
			if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
			{
				guizmoOperation = ImGuizmo::NO_OPERATION;
			}
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
	App->gui->SaveDialogAt(ASSETS_FOLDER, "tesseractScene");
}

void ModuleScene::wantToLoadScene()
{
	App->gui->LoadDialogAt(ASSETS_FOLDER, "tesseractScene");
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

	gameObjects.clear(); //Just stores pointers, the gameObjects will be deleted bellow

	std::list<GameObject*>::iterator it_ch;
	it_ch = root->childs.begin();
	while (it_ch != root->childs.end())
	{
		RELEASE((*it_ch));
		it_ch++;
	}
	root->childs.clear();
}

void ModuleScene::StartQuadTree()
{
	quadTree->Clear();
	quadTree->QT_Box = new AABB();
	quadTree->QT_Box->SetNegativeInfinity();

	for (std::list<GameObject*>::iterator it_ch = root->childs.begin(); it_ch != root->childs.end(); it_ch++)
	{
		ResizeQuadTree((*it_ch));
	}

	for (std::list<GameObject*>::iterator it_ch = root->childs.begin(); it_ch != root->childs.end(); it_ch++)
	{
		FillQuadtree((*it_ch));
	}
}

void ModuleScene::FillQuadtree(GameObject* gameObject)
{
	if (gameObject != nullptr && gameObject->isStatic)
	{
		quadTree->Insert(gameObject);	
	}
	for (std::list<GameObject*>::iterator it_c = gameObject->childs.begin(); it_c != gameObject->childs.end(); it_c++)
	{
		FillQuadtree((*it_c));
	}		
}

void ModuleScene::ResizeQuadTree(GameObject* gameObject)
{
	if (gameObject != nullptr && gameObject->isStatic)
	{
		quadTree->QT_Box->Enclose(gameObject->boundingBox);
	}
	for (std::list<GameObject*>::iterator it_c = gameObject->childs.begin(); it_c != gameObject->childs.end(); it_c++)
	{
		ResizeQuadTree((*it_c));
	}
}

void ModuleScene::DrawGuizmo(ImGuizmo::OPERATION operation)
{
	ComponentTransformation* transform = (ComponentTransformation*)App->scene_intro->selected_GO->GetComponent(TRANSFORMATION);

	if (transform != nullptr)
	{
		ImGuizmo::Enable(!selected_GO->isStatic);

		if (operation == ImGuizmo::NO_OPERATION)
		{
			ImGuizmo::Enable(false);
		}

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
			App->renderer3D->CalculateGlobalMatrix(root);
			root->RecalculateBB();
			StartQuadTree();
		}
	}
}

void ModuleScene::addGameObject(GameObject* gameObject, GameObject* parent)
{
	if (parent != nullptr)
	{
		parent->childs.push_back(gameObject);
		gameObject->parent = parent;
	}
	else
	{
		root->childs.push_back(gameObject);
		gameObject->parent = root;
	}

	gameObjects[gameObject->UID] = gameObject;
}

void ModuleScene::AddCamera()
{
	GameObject* newGameObject = new GameObject();
	App->scene_intro->addGameObject(newGameObject, App->scene_intro->root);
	newGameObject->name = "Camera";
	newGameObject->isStatic = false;

	ComponentTransformation* GOTransform = (ComponentTransformation*)newGameObject->AddComponent(TRANSFORMATION);

	float3 pos = float3::zero;
	float3 scale = float3::one;
	Quat rot = Quat::identity;

	GOTransform->position = pos;
	GOTransform->scale = scale;
	GOTransform->rotation = rot;
	GOTransform->localMatrix.Set(float4x4::FromTRS(pos, rot, scale));

	newGameObject->camera = (ComponentCamera*)newGameObject->AddComponent(CAMERA);
	newGameObject->camera->frustum.farPlaneDistance=100.0f;
	newGameObject->camera->frustum.verticalFov = DEGTORAD * 30.0f;
	newGameObject->camera->setAspectRatio(16.0f / 9.0f);


	newGameObject->boundingBox = newGameObject->camera->cameraBB;
}

void ModuleScene::AddEmptyGameObject()
{
	GameObject* newGameObject = new GameObject();
	App->scene_intro->addGameObject(newGameObject, App->scene_intro->root);
	newGameObject->name = "Empty Object";

	ComponentTransformation* GOTransform = (ComponentTransformation*)newGameObject->AddComponent(TRANSFORMATION);

	float3 pos = float3::zero;
	float3 scale = float3::one;
	Quat rot = Quat::identity;

	GOTransform->position = pos;
	GOTransform->scale = scale;
	GOTransform->rotation = rot;
	GOTransform->localMatrix.Set(float4x4::FromTRS(pos, rot, scale));

}

void ModuleScene::deleteGameObject(GameObject* GO)
{
	if (GO->parent != nullptr)
		GO->parent->childs.remove(GO);

	gameObjects.erase(GO->UID);

	RELEASE(GO);
}

void ModuleScene::selectGameObject(GameObject* gameObject)
{
	if (selected_GO != nullptr)
		selected_GO->setSelected(false);

	selected_GO = gameObject;
	if (gameObject != nullptr)
		gameObject->setSelected(true);
}

GameObject* ModuleScene::getGameObject(uint UID)
{
	return gameObjects[UID];
}
