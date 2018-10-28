#include "Application.h"
#include "ComponentCamera.h"


ComponentCamera::ComponentCamera(GameObject* gameobject, componentType type):Component(gameObject,type)
{
	frustum = new Frustum();

	frustum->SetPos({ 0.0f,30.0f,80.0f });
	frustum->SetFront({ 0.0f,0.0f,-1.0f });
	frustum->SetUp({ 0.0f,1.0f,0.0f });
	frustum->SetViewPlaneDistances(1.0f, 100.0f);
	frustum->SetVerticalFovAndAspectRatio(DEGTORAD*90.0f, 16.0f / 9.0f);
}

ComponentCamera::~ComponentCamera()
{
}

float * ComponentCamera::getViewMatrix()
{
	float4x4 matrix;
	matrix = frustum->ViewMatrix();
	matrix.Transpose();

	return (float*)matrix.v;
}

float * ComponentCamera::getViewProjMatrix()
{
	float4x4 matrix;
	matrix = frustum->ViewProjMatrix();
	matrix.Transpose();

	return (float*)matrix.v;
}
