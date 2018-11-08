#include "ComponentTransformation.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "GameObject.h"
#include "ModuleScene.h"

ComponentTransformation::~ComponentTransformation()
{
}

void ComponentTransformation::DrawInfo()
{
	if (ImGui::CollapsingHeader("Transformation", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_AllowItemOverlap))
	{
		if (gameObject->isStatic && ImGui::IsItemHovered())
			ImGui::SetTooltip("Object is static");

		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;

		if (gameObject->isStatic)
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, { 1.0f,0.6f,0.0f,0.1f });
			ImGui::PushStyleColor(ImGuiCol_Text, { 0.5,0.5,0.5f,1.0f });
			flags |= ImGuiInputTextFlags_ReadOnly;
		}

		ImGui::PushItemWidth(75);
		ImGui::Text("Position:");
		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::PushID("1");
		if (ImGui::InputFloat("", &position.x, 0.0f, 0.0f, "%.3f", flags))
			changed = true;
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::PushID("2");
		if (ImGui::InputFloat("", &position.y, 0.0f, 0.0f, "%.3f", flags))
			changed = true;
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::PushID("3");
		if (ImGui::InputFloat("", &position.z, 0.0f, 0.0f, "%.3f", flags))
			changed = true;
		ImGui::PopID();

		float3 rot = rotation.ToEulerXYZ();
		rot *= RADTODEG;
		ImGui::Text("Rotation:");
		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::PushID("4");
		if (ImGui::InputFloat("", &rot.x, 0.0f, 0.0f, "%.3f", flags))
			changed = true;
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::PushID("5");
		if (ImGui::InputFloat("", &rot.y, 0.0f, 0.0f, "%.3f", flags))
			changed = true;
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::PushID("6");
		if (ImGui::InputFloat("", &rot.z, 0.0f, 0.0f, "%.3f", flags))
			changed = true;
		ImGui::PopID();
		rot *= DEGTORAD;
		rotation = rotation.FromEulerXYZ(rot.x, rot.y, rot.z);

		ImGui::Text("Scale:");
		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::PushID("7");
		if (ImGui::InputFloat("", &scale.x, 0.0f, 0.0f, "%.3f", flags))
			changed = true;
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::PushID("8");
		if (ImGui::InputFloat("", &scale.y, 0.0f, 0.0f, "%.3f", flags))
			changed = true;
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::PushID("9");
		if (ImGui::InputFloat("", &scale.z, 0.0f, 0.0f, "%.3f", flags))
			changed = true;
		ImGui::PopID();
		ImGui::PopItemWidth();

		if (gameObject->isStatic)
		{
			ImGui::PopStyleColor(2);
		}

		if (changed)
		{
			RecalculateMatrix();
		}
	}
}
bool ComponentTransformation::DrawExtraConfig()
{
	if (ImGui::Button("Reset transformation"))
	{
		changed = true;

		position = { 0.0f,0.0f,0.0f };
		rotation = rotation.FromEulerXYZ(0.0f,0.0f,0.0f);
		scale = { 1.0f,1.0f,1.0f };

		RecalculateMatrix();		
		closeConfig = false;
	}

	return true;
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
