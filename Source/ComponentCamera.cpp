#include "Application.h"
#include "ComponentCamera.h"


ComponentCamera::ComponentCamera(GameObject* gameobject, componentType type) :Component(gameObject, type)
{
	frustum.type = math::FrustumType::PerspectiveFrustum;

	frustum.pos = { 0.0f,30.0f,80.0f };
	frustum.front = { 0.0f,0.0f,-1.0f };
	frustum.up = { 0.0f,1.0f,0.0f };

	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 1000.0f;

	frustum.verticalFov = DEGTORAD * 90.0f;
	setAspectRatio(16.0f / 9.0f);
}

ComponentCamera::~ComponentCamera()
{
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
	frustum.horizontalFov = 2.f * Atan(Tan(frustum.verticalFov*0.5f)*aspectRatio);
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