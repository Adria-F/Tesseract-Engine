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