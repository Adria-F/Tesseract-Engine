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

ModuleCamera3D::ModuleCamera3D(bool start_enabled)
{
	
}

ModuleCamera3D::~ModuleCamera3D()
{}

bool ModuleCamera3D::Init(JSON_File* document)
{
	camera = new ComponentCamera(nullptr, CAMERA);

	JSON_Value* cameraConf = document->getValue("camera");
	if (cameraConf != nullptr)
	{
		X = vec(cameraConf->getVector("X", 3)[0], cameraConf->getVector("X", 3)[1], cameraConf->getVector("X", 3)[2]);
		Y = vec(cameraConf->getVector("Y", 3)[0], cameraConf->getVector("Y", 3)[1], cameraConf->getVector("Y", 3)[2]);
		Z = vec(cameraConf->getVector("Z", 3)[0], cameraConf->getVector("Z", 3)[1], cameraConf->getVector("Z", 3)[2]);

		Position = vec(cameraConf->getVector("position", 3)[0], cameraConf->getVector("position", 3)[1], cameraConf->getVector("position", 3)[2]);
		Reference = vec(cameraConf->getVector("reference", 3)[0], cameraConf->getVector("reference", 3)[1], cameraConf->getVector("reference", 3)[2]);
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
			float DeltaX = (float)dx * mouseSensitivity*mouseSensitivity;

			newPos += X*DeltaX;
		}
		if (dy != 0)
		{
			float DeltaY = (float)dy * mouseSensitivity*mouseSensitivity;

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

bool ModuleCamera3D::Save(JSON_File* document)const
{
	JSON_Value* camera = document->createValue();
	camera->addString("name", "camera");
	document->addValue("camera", camera);

	return true;
}
bool ModuleCamera3D::Load(JSON_File* document)
{
	return true;
}