#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleGUI.h"
#include "Component.h"
#include "ComponentCamera.h"

ModuleCamera3D::ModuleCamera3D(bool start_enabled)
{
	
}

ModuleCamera3D::~ModuleCamera3D()
{}

bool ModuleCamera3D::Init(JSON_File* document)
{
	CalculateViewMatrix();

	JSON_Value* cameraConf = document->getValue("camera");
	if (cameraConf != nullptr)
	{
		X = vec3(cameraConf->getVector("X", 3)[0], cameraConf->getVector("X", 3)[1], cameraConf->getVector("X", 3)[2]);
		Y = vec3(cameraConf->getVector("Y", 3)[0], cameraConf->getVector("Y", 3)[1], cameraConf->getVector("Y", 3)[2]);
		Z = vec3(cameraConf->getVector("Z", 3)[0], cameraConf->getVector("Z", 3)[1], cameraConf->getVector("Z", 3)[2]);

		Position = vec3(cameraConf->getVector("position", 3)[0], cameraConf->getVector("position", 3)[1], cameraConf->getVector("position", 3)[2]);
		Reference = vec3(cameraConf->getVector("reference", 3)[0], cameraConf->getVector("reference", 3)[1], cameraConf->getVector("reference", 3)[2]);
		LookAt(Reference);

		cameraSpeed = cameraConf->getFloat("cameraSpeed");
		mouseSensitivity = cameraConf->getFloat("mouseSensitivity");
		wheelSensitivity = cameraConf->getFloat("wheelSensitivity");
		zoomDistance = cameraConf->getFloat("zoomDistance");
	}

	camera = new ComponentCamera(nullptr, CAMERA);


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
		vec3 newPos(0, 0, 0);
		float speed = cameraSpeed * dt;
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			speed *= 2;
		else if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT)
			speed /= 2;

		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) newPos.y += speed;
		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) newPos.y -= speed;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;


		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;

		Position += newPos;
		Reference += newPos;

		Reference = Position - getMovementFactor();
	}
	
	// Rotation over object ----------------
	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		Position = Reference + getMovementFactor();
	}

	if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE)==KEY_REPEAT)
	{
		vec3 newPos(0, 0, 0);
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
		vec3 newPos(0, 0, 0);
		float Sensitivity = wheelSensitivity;
		vec3 distance= Reference - Position;

		if (length(distance)<zoomDistance)
		{
			Sensitivity = length(distance) / zoomDistance;
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

	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

void ModuleCamera3D::FitCamera(const AABB &boundingBox)
{
	vec diagonal = boundingBox.Diagonal();
	vec center = boundingBox.CenterPoint();
	Position.z = (center.z+ diagonal.Length());
	Position.y = center.y;
	Position.x = center.x;
	LookAt({ center.x,center.y,center.z });
	//CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

vec3 ModuleCamera3D::getMovementFactor()
{
	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();

	vec3 newPosition = Position - Reference;

	if (dx != 0)
	{
		float DeltaX = (float)dx * mouseSensitivity;

		X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
	}

	if (dy != 0)
	{
		float DeltaY = (float)dy * mouseSensitivity;

		Y = rotate(Y, DeltaY, X);
		Z = rotate(Z, DeltaY, X);

		if (Y.y < 0.0f)
		{
			Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
			Y = cross(Z, X);
		}
	}
	
	return Z * length(newPosition);
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
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