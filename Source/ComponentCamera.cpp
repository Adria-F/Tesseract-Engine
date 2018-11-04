#include "Application.h"
#include "GameObject.h"
#include "ComponentCamera.h"


ComponentCamera::ComponentCamera(GameObject* gameobject, componentType type) :Component(gameObject, type)
{
	frustum.type = math::FrustumType::PerspectiveFrustum;

	frustum.pos = { -10.0f,5.0f,-9.0f };
	frustum.front = { 0.0f,0.0f,-1.0f };
	frustum.up = { 0.0f,1.0f,0.0f };

	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 100.0f;

	frustum.verticalFov = DEGTORAD * 90.0f;
	setAspectRatio(16.0f / 9.0f);

	cameraBB = AABB({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });

}

ComponentCamera::~ComponentCamera()
{
}

bool ComponentCamera::Update()
{
	if (!active)
		return false;

	float3 corner[8];
	frustum.GetCornerPoints(corner);
	
	cameraBB.SetNegativeInfinity();
	cameraBB.Enclose(corner, 8);
	
	DrawFrustum();
	CameraBB();
	return true;
}

float * ComponentCamera::getViewMatrix()
{
	static float4x4 matrix;
	matrix = frustum.ViewMatrix();
	matrix.Transpose();

	return (float*)matrix.v;
}

float * ComponentCamera::getProjectionMatrix()
{
	static float4x4 matrix;
	matrix = frustum.ProjectionMatrix();
	matrix.Transpose();

	return (float*)matrix.v;
}

void ComponentCamera::setAspectRatio(float aspectRatio)
{
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov*0.5f)*aspectRatio);
}

void ComponentCamera::DrawFrustum()
{
	float3 corners[8];
	frustum.GetCornerPoints(corners);

	glLineWidth(2.0f);
	//glColor3f(color.x, color.y, color.z);

	glBegin(GL_LINES);

	glVertex3f(corners[0].x, corners[0].y, corners[0].z);
	glVertex3f(corners[1].x, corners[1].y, corners[1].z);

	glVertex3f(corners[0].x, corners[0].y, corners[0].z);
	glVertex3f(corners[2].x, corners[2].y, corners[2].z);

	glVertex3f(corners[0].x, corners[0].y, corners[0].z);
	glVertex3f(corners[4].x, corners[4].y, corners[4].z);

	glVertex3f(corners[1].x, corners[1].y, corners[1].z);
	glVertex3f(corners[3].x, corners[3].y, corners[3].z);

	glVertex3f(corners[1].x, corners[1].y, corners[1].z);
	glVertex3f(corners[5].x, corners[5].y, corners[5].z);

	glVertex3f(corners[2].x, corners[2].y, corners[2].z);
	glVertex3f(corners[3].x, corners[3].y, corners[3].z);

	glVertex3f(corners[2].x, corners[2].y, corners[2].z);
	glVertex3f(corners[6].x, corners[6].y, corners[6].z);

	glVertex3f(corners[3].x, corners[3].y, corners[3].z);
	glVertex3f(corners[7].x, corners[7].y, corners[7].z);

	glVertex3f(corners[4].x, corners[4].y, corners[4].z);
	glVertex3f(corners[5].x, corners[5].y, corners[5].z);

	glVertex3f(corners[4].x, corners[4].y, corners[4].z);
	glVertex3f(corners[6].x, corners[6].y, corners[6].z);

	glVertex3f(corners[5].x, corners[5].y, corners[5].z);
	glVertex3f(corners[7].x, corners[7].y, corners[7].z);

	glVertex3f(corners[6].x, corners[6].y, corners[6].z);
	glVertex3f(corners[7].x, corners[7].y, corners[7].z);

	glEnd();

	glColor3f(1, 1, 1);
	glLineWidth(1.0f);
}

void ComponentCamera::DrawInfo()
{
	ImGui::PushID("toggleCamera");
	ImGui::Checkbox("", &active);
	ImGui::PopID();
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
	{
		ImGui::InputFloat("Near Plane", &frustum.nearPlaneDistance);
		ImGui::InputFloat("Far Plane", &frustum.farPlaneDistance);
		ImGui::NewLine();

		float AR = frustum.AspectRatio();
		float VFOV = frustum.verticalFov*RADTODEG;
		if (ImGui::SliderFloat("FOV", &VFOV, 10, 179))
		{
			frustum.verticalFov = VFOV*DEGTORAD;
			setAspectRatio(AR);
		}
		ImGui::Text(std::to_string(frustum.horizontalFov*RADTODEG).c_str());
		if (ImGui::InputFloat("Aspect Ratio", &AR, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
		{
			setAspectRatio(AR);
		}
	}
}

bool ComponentCamera::ContainsAABB(const AABB& boundingBox)
{
	Plane planes[6];
	float3 corners[8];
	int counter = 0;

	boundingBox.GetCornerPoints(corners);
	frustum.GetPlanes(planes);

	for (int i = 0; i < 6; i++)
	{
		//This number checks if the bb is outside of a plane
		int aux_count = counter;

		for (int j = 0; j < 8; j++)
		{
			if (!planes[i].IsOnPositiveSide(corners[j]))
			{
				counter++;
				break;
			}
		}
		if (aux_count == counter)
		{
			return false;
		}
	}

	if (counter == 6)
	{
		return true;
	}

	return false;
}

void ComponentCamera::Save(JSON_Value * component) const
{
	JSON_Value* camera = component->createValue();

	camera->addInt("Type", type);
	
	camera->addInt("Frustum Type", frustum.type);

	camera->addVector3("Position", frustum.pos);
	camera->addVector3("Front", frustum.front);
	camera->addVector3("Up", frustum.up);
	
	camera->addFloat("Near Plane", frustum.nearPlaneDistance);
	camera->addFloat("Far Plane", frustum.farPlaneDistance);

	camera->addFloat("VFOV", frustum.verticalFov);
	camera->addFloat("Aspect Ratio", frustum.AspectRatio());

	component->addValue("", camera);
}

void ComponentCamera::Load(JSON_Value * component)
{
	frustum.type = (math::FrustumType)component->getInt("Frustum Type");

	frustum.pos = component->getVector3("Position");
	frustum.front = component->getVector3("Front");
	frustum.up = component->getVector3("Up");

	frustum.nearPlaneDistance = component->getFloat("Near Plane");
	frustum.farPlaneDistance = component->getFloat("Far Plane");

	frustum.verticalFov = component->getFloat("VFOV");
	setAspectRatio(component->getFloat("Aspect Ratio"));
}

void ComponentCamera::CameraBB()
{
	glLineWidth(1.5f);
	glColor3f(1, 0.5f, 1);

	glBegin(GL_LINES);

	glVertex3f(cameraBB.CornerPoint(0).x, cameraBB.CornerPoint(0).y, cameraBB.CornerPoint(0).z);
	glVertex3f(cameraBB.CornerPoint(1).x, cameraBB.CornerPoint(1).y, cameraBB.CornerPoint(1).z);

	glVertex3f(cameraBB.CornerPoint(0).x, cameraBB.CornerPoint(0).y, cameraBB.CornerPoint(0).z);
	glVertex3f(cameraBB.CornerPoint(2).x, cameraBB.CornerPoint(2).y, cameraBB.CornerPoint(2).z);

	glVertex3f(cameraBB.CornerPoint(0).x, cameraBB.CornerPoint(0).y, cameraBB.CornerPoint(0).z);
	glVertex3f(cameraBB.CornerPoint(4).x, cameraBB.CornerPoint(4).y, cameraBB.CornerPoint(4).z);

	glVertex3f(cameraBB.CornerPoint(3).x, cameraBB.CornerPoint(3).y, cameraBB.CornerPoint(3).z);
	glVertex3f(cameraBB.CornerPoint(1).x, cameraBB.CornerPoint(1).y, cameraBB.CornerPoint(1).z);

	glVertex3f(cameraBB.CornerPoint(3).x, cameraBB.CornerPoint(3).y, cameraBB.CornerPoint(3).z);
	glVertex3f(cameraBB.CornerPoint(2).x, cameraBB.CornerPoint(2).y, cameraBB.CornerPoint(2).z);

	glVertex3f(cameraBB.CornerPoint(3).x, cameraBB.CornerPoint(3).y, cameraBB.CornerPoint(3).z);
	glVertex3f(cameraBB.CornerPoint(7).x, cameraBB.CornerPoint(7).y, cameraBB.CornerPoint(7).z);

	glVertex3f(cameraBB.CornerPoint(6).x, cameraBB.CornerPoint(6).y, cameraBB.CornerPoint(6).z);
	glVertex3f(cameraBB.CornerPoint(2).x, cameraBB.CornerPoint(2).y, cameraBB.CornerPoint(2).z);

	glVertex3f(cameraBB.CornerPoint(6).x, cameraBB.CornerPoint(6).y, cameraBB.CornerPoint(6).z);
	glVertex3f(cameraBB.CornerPoint(4).x, cameraBB.CornerPoint(4).y, cameraBB.CornerPoint(4).z);

	glVertex3f(cameraBB.CornerPoint(6).x, cameraBB.CornerPoint(6).y, cameraBB.CornerPoint(6).z);
	glVertex3f(cameraBB.CornerPoint(7).x, cameraBB.CornerPoint(7).y, cameraBB.CornerPoint(7).z);

	glVertex3f(cameraBB.CornerPoint(5).x, cameraBB.CornerPoint(5).y, cameraBB.CornerPoint(5).z);
	glVertex3f(cameraBB.CornerPoint(1).x, cameraBB.CornerPoint(1).y, cameraBB.CornerPoint(1).z);

	glVertex3f(cameraBB.CornerPoint(5).x, cameraBB.CornerPoint(5).y, cameraBB.CornerPoint(5).z);
	glVertex3f(cameraBB.CornerPoint(4).x, cameraBB.CornerPoint(4).y, cameraBB.CornerPoint(4).z);

	glVertex3f(cameraBB.CornerPoint(5).x, cameraBB.CornerPoint(5).y, cameraBB.CornerPoint(5).z);
	glVertex3f(cameraBB.CornerPoint(7).x, cameraBB.CornerPoint(7).y, cameraBB.CornerPoint(7).z);

	glEnd();

	glColor3f(1, 1, 1);
	glLineWidth(1.0f);
}
