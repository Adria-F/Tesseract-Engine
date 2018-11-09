#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleGUI.h"
#include "Component.h"
#include "ComponentCamera.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentTransformation.h"

ModuleCamera3D::ModuleCamera3D(bool start_enabled)
{
	
}

ModuleCamera3D::~ModuleCamera3D()
{}

bool ModuleCamera3D::Init(JSON_File* document)
{
	camera = new ComponentCamera(nullptr, CAMERA);
	camera->active = false;

	JSON_Value* cameraConf = document->getValue("camera");
	if (cameraConf != nullptr)
	{
		X = cameraConf->getVector3("X");
		Y = cameraConf->getVector3("Y");
		Z = cameraConf->getVector3("Z");

		Position = cameraConf->getVector3("position");
		Reference = cameraConf->getVector3("reference");
		LookAt(Reference);

		cameraSpeed = cameraConf->getFloat("cameraSpeed");
		mouseSensitivity = cameraConf->getFloat("mouseSensitivity");
		wheelSensitivity = cameraConf->getFloat("wheelSensitivity");
		zoomDistance = cameraConf->getFloat("zoomDistance");
	}

	return true;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		vec newPos(0, 0, 0);
		float speed = cameraSpeed * dt;
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			speed *= 2;
		else if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT)
			speed /= 2;

		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) newPos.y += speed;
		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) newPos.y -= speed;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos += camera->frustum.front*speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos -= camera->frustum.front*speed;


		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= camera->frustum.WorldRight()*speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += camera->frustum.WorldRight()*speed;

		Position += newPos;
		Reference += newPos;

		Reference = Position - getMovementFactor();
	}
	
	// Rotation over object ----------------
	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		camera->frustum.pos = Position = Reference + getMovementFactor();
	}

	if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE)==KEY_REPEAT)
	{
		vec newPos(0, 0, 0);
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();
		
		if (dx != 0)
		{
			float DeltaX = (float)dx * mouseSensitivity;

			newPos += X*DeltaX;
		}
		if (dy != 0)
		{
			float DeltaY = (float)dy * mouseSensitivity;

			newPos -= Y * DeltaY;
		}

		Position += newPos;
		Reference += newPos;
	}

	if (App->input->GetMouseZ()!=0)
	{
		vec newPos(0, 0, 0);
		float Sensitivity = wheelSensitivity;
		vec vec_distance= Reference - Position;

		if (vec_distance.Length()<zoomDistance)
		{
			Sensitivity = vec_distance.Length() / zoomDistance;
		}

		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			Sensitivity = 2;

		if (App->input->GetMouseZ() > 0)
		{
			newPos -= Z * Sensitivity;
		}
		else
		{
			newPos += Z * Sensitivity;
		}

		Position += newPos;
	}

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT)
	{
		LookAt({ 0,0,0 });
	}

	camera->frustum.pos=Position;

	Z = -camera->frustum.front;
	Y = camera->frustum.up;
	X = camera->frustum.WorldRight();


	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT && !ImGuizmo::IsOver())
	{
		App->scene_intro->selectGameObject(checkMousePick());
	}

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec &Position, const vec &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = (Position - Reference).Normalized();
	X = (vec(0.0f, 1.0f, 0.0f).Cross(Z)).Normalized();
	Y = Z.Cross(X);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const vec &Spot)
{
	Reference = Spot;

	//caluclate direction to look
	vec dir = Spot - camera->frustum.pos;

	//caluclate the new view matrix
	float3x3 viewMat = float3x3::LookAt(camera->frustum.front, dir.Normalized(), camera->frustum.up, vec(0.0f, 1.0f, 0.0f));

	//set new front and up for the frustum
	camera->frustum.front = viewMat.MulDir(camera->frustum.front).Normalized();
	camera->frustum.up = viewMat.MulDir(camera->frustum.up).Normalized();
}

// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec &Movement)
{
	Position += Movement;
	Reference += Movement;
}

void ModuleCamera3D::FitCamera(const AABB &boundingBox)
{
	vec diagonal = boundingBox.Diagonal();
	vec center = boundingBox.CenterPoint();
	Position.z = camera->frustum.pos.z = (center.z+ diagonal.Length());
	Position.y = camera->frustum.pos.y = center.y;
	Position.x = camera->frustum.pos.x = center.x;
	LookAt({ center.x,center.y,center.z });
}

vec ModuleCamera3D::getMovementFactor()
{
	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();

	vec newPosition = Position - Reference;

	if (dx != 0)
	{
		float DeltaX = (float)dx * mouseSensitivity;

		Quat rotation = Quat::RotateY(DeltaX);
		camera->frustum.front = rotation.Mul(camera->frustum.front).Normalized();
		camera->frustum.up = rotation.Mul(camera->frustum.up).Normalized();
	}

	if (dy != 0)
	{
		float DeltaY = (float)dy * mouseSensitivity;
		
		Quat rotation = Quat::RotateAxisAngle(camera->frustum.WorldRight(),DeltaY);

		if (rotation.Mul(camera->frustum.up).Normalized().y > 0.0f)
		{
			camera->frustum.up = rotation.Mul(camera->frustum.up).Normalized();
			camera->frustum.front = rotation.Mul(camera->frustum.front).Normalized();
		}
	}
	
	return -camera->frustum.front * newPosition.Length();
}

GameObject* ModuleCamera3D::checkMousePick()
{
	GameObject* ret = nullptr;

	float mouseX = App->input->GetMouseX() - App->gui->sceneX;
	float mouseY = App->input->GetMouseY() - App->gui->sceneY;
	mouseX = (mouseX / (App->gui->sceneW / 2)) - 1;
	mouseY = (mouseY / (App->gui->sceneH / 2)) - 1;
	LineSegment ray = camera->frustum.UnProjectLineSegment(mouseX, -mouseY);
	App->renderer3D->clickA = ray.a;
	App->renderer3D->clickB = ray.b;

	//Fill queue
	std::priority_queue<HitGameObject*, std::vector<HitGameObject*>, OrderCrit> gameObjects;
	fillHitGameObjects(App->scene_intro->root, gameObjects, ray);

	if (gameObjects.size() > 0)
		ret = checkCloserGameObjects(gameObjects, ray);

	return ret;
}

void ModuleCamera3D::fillHitGameObjects(GameObject* current, std::priority_queue<HitGameObject*, std::vector<HitGameObject*>, OrderCrit>& gameObjects, LineSegment ray)
{
	float distance, exitDistance;
	if (ray.Intersects(current->boundingBox, distance, exitDistance))
	{
		HitGameObject* hit = new HitGameObject(current, distance);
		gameObjects.push(hit);
	}

	for (std::list<GameObject*>::iterator it_c = current->childs.begin(); it_c != current->childs.end(); it_c++)
	{
		fillHitGameObjects((*it_c), gameObjects, ray);
	}
}

GameObject* ModuleCamera3D::checkCloserGameObjects(std::priority_queue<HitGameObject*, std::vector<HitGameObject*>, OrderCrit>& queue, LineSegment ray, float distance)
{
	GameObject* ret = nullptr;

	HitGameObject* curr = queue.top();
	queue.pop();
	float TriDistance = hitsTriangle(curr->GO, ray);
	if (TriDistance != -1 && (TriDistance < distance || distance == -1))
	{
		distance = TriDistance;
		ret = curr->GO;
	}
	if (queue.size() > 0 && (queue.top()->distance < distance || distance == -1))
	{
		GameObject* GO2 = checkCloserGameObjects(queue, ray, distance);
		if (GO2 != nullptr)
			ret = GO2;
	}
	RELEASE(curr);

	return ret;
}

float ModuleCamera3D::hitsTriangle(GameObject* gameObject, LineSegment ray)
{
	float smallestDistance = -1.0f;

	ComponentTransformation* transformation = (ComponentTransformation*)gameObject->GetComponent(TRANSFORMATION);
	if (transformation != nullptr)
		ray.Transform(transformation->globalMatrix.Inverted());

	ComponentMesh* mesh = (ComponentMesh*)gameObject->GetComponent(MESH);
	if (mesh != nullptr)
	{		
		for (int i = 0; i < mesh->mesh->num_indices; i+=3)
		{
			math::Triangle tri;
			tri.a = { mesh->mesh->vertices[mesh->mesh->indices[i] * 3],mesh->mesh->vertices[mesh->mesh->indices[i] * 3 + 1],mesh->mesh->vertices[mesh->mesh->indices[i] * 3 + 2] };
			tri.b = { mesh->mesh->vertices[mesh->mesh->indices[i + 1] * 3],mesh->mesh->vertices[mesh->mesh->indices[i + 1] * 3 + 1],mesh->mesh->vertices[mesh->mesh->indices[i + 1] * 3 + 2] };
			tri.c = { mesh->mesh->vertices[mesh->mesh->indices[i + 2] * 3],mesh->mesh->vertices[mesh->mesh->indices[i + 2] * 3 + 1],mesh->mesh->vertices[mesh->mesh->indices[i + 2] * 3 + 2] };
		
			float distance;
			float3 intPoint;
			bool hit = ray.Intersects(tri, &distance, &intPoint);
			if (distance > 0 && (distance < smallestDistance || smallestDistance == -1.0f))
			{
				smallestDistance = distance;
			}
		}		
	}

	return smallestDistance;
}

bool ModuleCamera3D::SaveDefaultConfig(JSON_File* document)const
{
	JSON_Value* camera = document->createValue();
	
	camera->addVector3("position", { 0.0f,30.0f,80.0f });
	camera->addVector3("reference", float3::zero);
	camera->addVector3("X", float3::unitX);
	camera->addVector3("Y", float3::unitY);
	camera->addVector3("Z", float3::unitZ);
	camera->addFloat("cameraSpeed", 10.0f);
	camera->addFloat("mouseSensitivity", 0.007);
	camera->addFloat("wheelSensitivity", 10.0f);
	camera->addFloat("zoomDistance", 20.0f);

	document->addValue("camera", camera);

	return true;
}