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

//TEST to change animations
#include "ComponentAnimation.h"
#include "Resource.h"
#include "ResourceAnimation.h"

#ifdef _DEBUG
//#define TEST_MEMORY_MANAGER
#include "mmgr/mmgr.h"
#endif

bool Same_GameObject(GameObject* first, GameObject* second)
{
	return (first == second);
}

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
	root->culling = true;
	root->AddComponent(TRANSFORMATION);

	quadTree = new Quadtree();
	StartQuadTree();

	//Load Street scene
	App->scene_loader->loadSceneFile("Assets/Scenes/Walk");
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

	root->Update(App->game_dt); //Update gameObjects

	std::map<uint, GameObject*>::iterator it_go = gameObjects.begin();
	while (it_go != gameObjects.end())
	{
		if ((*it_go).second->to_delete)
		{
			deleteGameObject((*it_go).second);
			it_go = gameObjects.begin();
		}
		else
		{
			ComponentAnimation* anim = (ComponentAnimation*)(*it_go).second->GetComponent(ANIMATION);

			it_go++;
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

void ModuleScene::FillDrawBuffer(bool game)
{
	ComponentCamera* camera = (game&&activeCamera != nullptr) ? activeCamera->camera : App->camera->camera;

	if ((camera == App->camera->camera && App->renderer3D->Frustum_Culling) || (activeCamera != nullptr && camera == activeCamera->camera && activeCamera->camera->IsActive))
	{
		//Static objects-------------------------------------------------------------------
		if (quadTree->QT_Box != nullptr)
		{
			//Fill the vector of the objects inside the same quads of the camera's bb
			quadTree->Intersect(ObjectsToDraw, camera->frustum);
			ObjectsToDraw.sort();
			ObjectsToDraw.unique(Same_GameObject);

			//From the possible objects only draw the ones inside the frustum
			for (list<GameObject*>::iterator Otd_it = ObjectsToDraw.begin(); Otd_it != ObjectsToDraw.end(); Otd_it++)
			{
				if (camera->ContainsAABB((*Otd_it)->boundingBox))
				{
					(*Otd_it)->culling = true;
				}
				else
				{
					(*Otd_it)->culling = false;
				}
			}
		}

		//Non-Static objects-------------------------------------------------------------------

		//From the possible objects only draw the ones inside the frustum
		for (std::map<uint, GameObject*>::iterator it_ch = gameObjects.begin(); it_ch != gameObjects.end(); it_ch++)
		{
			if (!(*it_ch).second->isStatic)
			{
				if (camera->ContainsAABB((*it_ch).second->boundingBox) && (*it_ch).second->GetComponent(CAMERA) == nullptr)
				{
					(*it_ch).second->culling = true;
				}
				else
				{
					(*it_ch).second->culling = false;
				}
				if ((*it_ch).second->GetComponent(CAMERA) != nullptr)
				{
					(*it_ch).second->culling = true;
				}
			}
		}
	}

	root->Draw(game); //Add gameObjects to renderBuffer

	for (std::map<uint, GameObject*>::iterator it_ch = gameObjects.begin(); it_ch != gameObjects.end(); it_ch++)
	{
		(*it_ch).second->culling = false;
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

	if (App->renderer3D->ShowQT && !game)
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
	activeCamera = nullptr;

	LOG("Unloading scene");
	gameObjects.clear(); //Just stores pointers, the gameObjects will be deleted bellow
	cameras.clear();

	std::list<GameObject*>::iterator it_ch;
	it_ch = root->childs.begin();
	while (it_ch != root->childs.end())
	{
		RELEASE((*it_ch));
		it_ch++;
	}
	root->childs.clear();

	App->renderer3D->renderBuffer.clear();
	while (App->renderer3D->blendColorsBuffer.size() > 0)
	{
		App->renderer3D->blendColorsBuffer.pop();
	}
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

		float4x4* ViewMatrix = (float4x4*)App->camera->camera->getViewMatrix();
		float4x4*ProjectionMatrix = (float4x4*)App->camera->camera->getProjectionMatrix();

		ImGuizmo::MODE mode;

		float4x4* GlobalMat;
		GlobalMat = &transform->globalMatrix;

		float3 scale = float3::one;
		float3 pos;
		Quat rot;
		if (operation != ImGuizmo::OPERATION::SCALE)
		{			
			GlobalMat->Decompose(pos, rot, scale);
			GlobalMat->Set(float4x4::FromTRS(pos, rot, float3::one));
		}
		GlobalMat->Transpose();

		ImGuizmo::SetOrthographic(false);

		ImGuizmo::Manipulate((float*)ViewMatrix, (float*)ProjectionMatrix, operation, ImGuizmo::LOCAL, (float*)GlobalMat,NULL,NULL);
		GlobalMat->Transpose();

		if (operation != ImGuizmo::OPERATION::SCALE)
		{
			float3 oneScale;
			GlobalMat->Decompose(pos, rot, oneScale);
			GlobalMat->Set(float4x4::FromTRS(pos, rot, scale));
		}

		if (ImGuizmo::IsUsing())
		{
			if (App->scene_intro->selected_GO->parent != nullptr)
			{
				transform->localMatrix = ((ComponentTransformation*)App->scene_intro->selected_GO->parent->GetComponent(TRANSFORMATION))->globalMatrix.Inverted()*transform->globalMatrix;

			}
			transform->localMatrix.Decompose(transform->position, transform->rotation, transform->scale);
			App->scene_intro->selected_GO->RecalculateBB(); //To avoid that the BB laggs to follow object position
			transform->changed = true;
		}
	}
}

void ModuleScene::addGameObject(GameObject* gameObject, GameObject* parent, GameObject* fakeScene)
{
	if (parent != nullptr)
	{
		parent->childs.push_back(gameObject);
		gameObject->parent = parent;
	}
	else
	{
		if (fakeScene == nullptr)
		{
			root->childs.push_back(gameObject);
			gameObject->parent = root;
		}
		else
		{
			fakeScene->childs.push_back(gameObject);
			gameObject->parent = fakeScene;
		}
	}

	if (fakeScene == nullptr)
	{
		std::vector<std::string*> names;
		for (map<uint, GameObject*>::iterator it_go = gameObjects.begin(); it_go != gameObjects.end(); it_go++)
		{
			names.push_back(&(*it_go).second->name);
		}
		App->fileSystem->getAvailableNameFromArray(names, gameObject->name);

		gameObjects[gameObject->UID] = gameObject;
	}
}

void ModuleScene::AddCamera()
{
	GameObject* newGameObject = new GameObject();
	newGameObject->name = "Camera";
	App->scene_intro->addGameObject(newGameObject, App->scene_intro->root);
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
	newGameObject->camera->RecalculateBB();

	newGameObject->boundingBox = newGameObject->camera->cameraBB;

	if (cameras.size() == 0)
		ChangeCulling(newGameObject);

	cameras.push_back(newGameObject);
}

void ModuleScene::AddEmptyGameObject()
{
	GameObject* newGameObject = new GameObject();
	newGameObject->name = "Empty Object";
	App->scene_intro->addGameObject(newGameObject, App->scene_intro->root);

	ComponentTransformation* GOTransform = (ComponentTransformation*)newGameObject->AddComponent(TRANSFORMATION);

	float3 pos = float3::zero;
	float3 scale = float3::one;
	Quat rot = Quat::identity;

	GOTransform->position = pos;
	GOTransform->scale = scale;
	GOTransform->rotation = rot;
	GOTransform->localMatrix.Set(float4x4::FromTRS(pos, rot, scale));

}

void ModuleScene::deleteGameObject(GameObject* GO, bool deleteFromParent)
{
	if (GO->parent != nullptr && deleteFromParent)
		GO->parent->childs.remove(GO);

	if (GO->camera != nullptr) //It is a camera
	{
		cameras.remove(GO);
		if (GO->camera->IsActive && cameras.size() > 0)
		{
			ChangeCulling(cameras.front(), true);
		}
		else if (cameras.size() == 0)
		{
			activeCamera = nullptr;
			App->renderer3D->Frustum_Culling = false;
		}
	}

	gameObjects.erase(GO->UID);
	App->renderer3D->removeFromRenderBuffer(GO);

	if (selected_GO == GO)
		selected_GO = nullptr;

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
	if (gameObjects.find(UID) != gameObjects.end())
		return gameObjects[UID];
	else
		return nullptr;
}

void ModuleScene::addComponent(Component* component)
{
	components[component->UID] = component;
}

Component * ModuleScene::getComponent(uint UID)
{
	if (components.find(UID) != components.end())
		return components[UID];
	else
		return nullptr;
}

void ModuleScene::removeComponent(Component* component)
{
	components.erase(component->UID);
}

void ModuleScene::FindCameras(GameObject* parent)
{
	for (std::list<GameObject*>::iterator go_it=parent->childs.begin();go_it!=parent->childs.end();go_it++)
	{
		if ((*go_it)->childs.size() > 0)
		{
			FindCameras((*go_it));
		}
		if ((*go_it)->camera != nullptr)
		{
			cameras.push_back(*go_it);
			if ((*go_it)->camera->IsActive)
				ChangeCulling((*go_it), true);
		}
	}
}

void ModuleScene::ChangeCulling(GameObject* GO, bool culling)
{
	if (activeCamera != nullptr)
		activeCamera->camera->IsActive = false;

	GO->camera->IsActive = culling;

	if (culling)
		activeCamera = GO;
	else if (activeCamera == GO)
	{
		activeCamera = nullptr;
		App->renderer3D->Frustum_Culling = false;
	}
}
