#include "ComponentTransformation.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "GameObject.h"

ComponentTransformation::~ComponentTransformation()
{
}

void ComponentTransformation::DrawInfo()
{
	if (ImGui::CollapsingHeader("Transformation", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
	{
		changed = false;

		ImGui::PushItemWidth(75);
		ImGui::Text("Position:");
		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::PushID("1");
		if (ImGui::InputFloat("", &position.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			changed = true;
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::PushID("2");
		if (ImGui::InputFloat("", &position.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			changed = true;
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::PushID("3");
		if (ImGui::InputFloat("", &position.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			changed = true;
		ImGui::PopID();

		float3 rot = rotation.ToEulerXYZ();
		rot *= RADTODEG;
		ImGui::Text("Rotation:");
		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::PushID("4");
		if (ImGui::InputFloat("", &rot.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			changed = true;
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::PushID("5");
		if (ImGui::InputFloat("", &rot.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			changed = true;
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::PushID("6");
		if (ImGui::InputFloat("", &rot.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			changed = true;
		ImGui::PopID();
		rot *= DEGTORAD;
		rotation = rotation.FromEulerXYZ(rot.x, rot.y, rot.z);

		ImGui::Text("Scale:");
		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::PushID("7");
		if (ImGui::InputFloat("", &scale.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			changed = true;
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::PushID("8");
		if (ImGui::InputFloat("", &scale.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			changed = true;
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::PushID("9");
		if (ImGui::InputFloat("", &scale.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			changed = true;
		ImGui::PopID();
		ImGui::PopItemWidth();

		if (changed)
		{
			RecalculateMatrix();
		}
	}
}
bool ComponentTransformation::Update()
{
	return true;
}

void ComponentTransformation::RecalculateMatrix()
{
	localMatrix.Set(float4x4::FromTRS(position, rotation, scale));
}

void ComponentTransformation::Save(JSON_Value * component) const
{
	JSON_Value* transformation = component->createValue();

	transformation->addInt("Type", type);
	transformation->addVector3("Position", position);
	transformation->addQuat("Rotation", rotation);
	transformation->addVector3("Scale", scale);
	transformation->addTransform("Transform", localMatrix);

	component->addValue("",transformation);
}

void ComponentTransformation::Load(JSON_Value * component)
{
	position = component->getVector3("Position");
	rotation = component->getQuat("Rotation");
	scale = component->getVector3("Scale");
	localMatrix = component->getTransform("Transform");
}
