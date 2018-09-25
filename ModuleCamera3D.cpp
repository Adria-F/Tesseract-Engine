#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModulePhysics3D.h"
#include "ModuleGUI.h"

ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module(start_enabled)
{
	CalculateViewMatrix();

	X = vec(1.0f, 0.0f, 0.0f);
	Y = vec(0.0f, 1.0f, 0.0f);
	Z = vec(0.0f, 0.0f, 1.0f);

	Position = vec(0.0f, 50.0f, 0.0f);
	Reference = vec(0.0f, 0.0f, 0.0f);

	offset_to_player = vec(0.0f, 14.0f, -12.5f);
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
	 //Implement a debug camera with keys and mouse
	 //Now we can make this movememnt frame rate independant!

	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN) freeCam = !freeCam;

	if (freeCam)
	{
		vec newPos(0, 0, 0);
		float speed = CAMERA_SPEED * dt;
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			speed *= 4;

		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) newPos.y += speed;
		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT) newPos.y -= speed;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;


		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;

		Position += newPos;
		Reference += newPos;

		// Mouse motion ----------------

		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		{
			int dx = -App->input->GetMouseXMotion();
			int dy = -App->input->GetMouseYMotion();

			float Sensitivity = 0.25f;

			Position -= Reference;

			if (dx != 0)
			{
				float DeltaX = (float)dx * Sensitivity;

				/*X = rotate(X, DeltaX, vec(0.0f, 1.0f, 0.0f));
				Y = rotate(Y, DeltaX, vec(0.0f, 1.0f, 0.0f));
				Z = rotate(Z, DeltaX, vec(0.0f, 1.0f, 0.0f));*/
			}

			if (dy != 0)
			{
				float DeltaY = (float)dy * Sensitivity;

				/*Y = rotate(Y, DeltaY, X);
				Z = rotate(Z, DeltaY, X);*/

				if (Y.y < 0.0f)
				{
					Z = vec(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
					Y = Z.Cross(X);
				}
			}

			Position = Reference + Z * Position.Length();
		}
	}

	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec &Position, const vec &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	vec aux_vec = Position - Reference;

	Z = aux_vec.Normalized();
	X = (vec(0.0f, 1.0f, 0.0f).Cross(Z)).Normalized();
	Y = Z.Cross(X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const vec &Spot)
{
	Reference = Spot;

	vec aux_vec = Position - Reference;

	Z = aux_vec.Normalized();
	X = (vec(0.0f, 1.0f, 0.0f).Cross(Z)).Normalized();
	Y = Z.Cross(X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float4x4* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

void ModuleCamera3D::setFreeCam(bool freeCam)
{
	this->freeCam = freeCam;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	
	ViewMatrix = float4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -X.Dot(Position), -Y.Dot(Position), -Z.Dot(Position), 1.0f);
	ViewMatrixInverse = ViewMatrix.Inverted();
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