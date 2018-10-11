#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleGUI.h"

ModuleCamera3D::ModuleCamera3D(bool start_enabled)
{
	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 30.0f, 80.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);
	LookAt(Reference);

}

ModuleCamera3D::~ModuleCamera3D()
{}

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
	Position.z = (center.z - diagonal.Length());
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

void ModuleCamera3D::renderTotalBB()
{
	glLineWidth(2.0f);
	glColor3f(1, 0.5f, 1);

	glBegin(GL_LINES);

	glVertex3f(BBtoLook->CornerPoint(0).x, BBtoLook->CornerPoint(0).y, BBtoLook->CornerPoint(0).z);
	glVertex3f(BBtoLook->CornerPoint(1).x, BBtoLook->CornerPoint(1).y, BBtoLook->CornerPoint(1).z);
	glVertex3f(BBtoLook->CornerPoint(0).x, BBtoLook->CornerPoint(0).y, BBtoLook->CornerPoint(0).z);
	glVertex3f(BBtoLook->CornerPoint(2).x, BBtoLook->CornerPoint(2).y, BBtoLook->CornerPoint(2).z);
	glVertex3f(BBtoLook->CornerPoint(0).x, BBtoLook->CornerPoint(0).y, BBtoLook->CornerPoint(0).z);
	glVertex3f(BBtoLook->CornerPoint(4).x, BBtoLook->CornerPoint(4).y, BBtoLook->CornerPoint(4).z);
	glVertex3f(BBtoLook->CornerPoint(3).x, BBtoLook->CornerPoint(3).y, BBtoLook->CornerPoint(3).z);
	glVertex3f(BBtoLook->CornerPoint(1).x, BBtoLook->CornerPoint(1).y, BBtoLook->CornerPoint(1).z);
	glVertex3f(BBtoLook->CornerPoint(3).x, BBtoLook->CornerPoint(3).y, BBtoLook->CornerPoint(3).z);
	glVertex3f(BBtoLook->CornerPoint(2).x, BBtoLook->CornerPoint(2).y, BBtoLook->CornerPoint(2).z);
	glVertex3f(BBtoLook->CornerPoint(3).x, BBtoLook->CornerPoint(3).y, BBtoLook->CornerPoint(3).z);
	glVertex3f(BBtoLook->CornerPoint(7).x, BBtoLook->CornerPoint(7).y, BBtoLook->CornerPoint(7).z);
	glVertex3f(BBtoLook->CornerPoint(6).x, BBtoLook->CornerPoint(6).y, BBtoLook->CornerPoint(6).z);
	glVertex3f(BBtoLook->CornerPoint(2).x, BBtoLook->CornerPoint(2).y, BBtoLook->CornerPoint(2).z);
	glVertex3f(BBtoLook->CornerPoint(6).x, BBtoLook->CornerPoint(6).y, BBtoLook->CornerPoint(6).z);
	glVertex3f(BBtoLook->CornerPoint(4).x, BBtoLook->CornerPoint(4).y, BBtoLook->CornerPoint(4).z);
	glVertex3f(BBtoLook->CornerPoint(6).x, BBtoLook->CornerPoint(6).y, BBtoLook->CornerPoint(6).z);
	glVertex3f(BBtoLook->CornerPoint(7).x, BBtoLook->CornerPoint(7).y, BBtoLook->CornerPoint(7).z);
	glVertex3f(BBtoLook->CornerPoint(5).x, BBtoLook->CornerPoint(5).y, BBtoLook->CornerPoint(5).z);
	glVertex3f(BBtoLook->CornerPoint(1).x, BBtoLook->CornerPoint(1).y, BBtoLook->CornerPoint(1).z);
	glVertex3f(BBtoLook->CornerPoint(5).x, BBtoLook->CornerPoint(5).y, BBtoLook->CornerPoint(5).z);
	glVertex3f(BBtoLook->CornerPoint(4).x, BBtoLook->CornerPoint(4).y, BBtoLook->CornerPoint(4).z);
	glVertex3f(BBtoLook->CornerPoint(5).x, BBtoLook->CornerPoint(5).y, BBtoLook->CornerPoint(5).z);
	glVertex3f(BBtoLook->CornerPoint(7).x, BBtoLook->CornerPoint(7).y, BBtoLook->CornerPoint(7).z);

	glEnd();

	glColor3f(1, 1, 1);
	glLineWidth(1.0f);

}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
}

bool ModuleCamera3D::Save(rapidjson::Document& document, rapidjson::FileWriteStream& os)
{
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
	document.AddMember("name", "camera", allocator);
	rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
	return true;
}
bool ModuleCamera3D::Load(rapidjson::Document& document)
{
	return true;
}