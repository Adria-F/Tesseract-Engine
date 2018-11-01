#include "ComponentTransformation.h"
#include "Application.h"

ComponentTransformation::~ComponentTransformation()
{
}

void ComponentTransformation::DrawInfo()
{
	if (ImGui::CollapsingHeader("Transformation", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
	{
		ImGui::PushItemWidth(75);
		ImGui::Text("Position:");
		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::PushID("1");
		ImGui::InputFloat("", &position.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue); ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::PushID("2");
		ImGui::InputFloat("", &position.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue); ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::PushID("3");
		ImGui::InputFloat("", &position.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::PopID();

		float3 rot = rotation.ToEulerXYZ();
		rot *= RADTODEG;
		ImGui::Text("Rotation:");
		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::PushID("4");
		ImGui::InputFloat("", &rot.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue); ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::PushID("5");
		ImGui::InputFloat("", &rot.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue); ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::PushID("6");
		ImGui::InputFloat("", &rot.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::PopID();
		rot *= DEGTORAD;
		rotation = rotation.FromEulerXYZ(rot.x, rot.y, rot.z);

		ImGui::Text("Scale:");
		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::PushID("7");
		ImGui::InputFloat("", &scale.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue); ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::PushID("8");
		ImGui::InputFloat("", &scale.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue); ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::PushID("9");
		ImGui::InputFloat("", &scale.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::PopID();
		ImGui::PopItemWidth();
	}
}
bool ComponentTransformation::Update()
{
	return true;
}

void ComponentTransformation::RecalculateMatrix()
{

}

void ComponentTransformation::Save(JSON_Value * component) const
{
	JSON_Value* transformation = component->createValue();

	transformation->addInt("Type", type);
	transformation->addVector3("Position", position);
	transformation->addQuat("Rotation", rotation);
	transformation->addVector3("Scale", scale);

	component->addValue("",transformation);
}

void ComponentTransformation::Load(JSON_Value * component)
{
	position = component->getVector3("Position");
	rotation = component->getQuat("Rotation");
	scale = component->getVector3("Scale");
}
