#include "Application.h"
#include "ComponentAnimation.h"
#include "ModuleResource.h"
#include "Resource.h"
#include "ResourceAnimation.h"
#include "GameObject.h"

#include "Component.h"
#include "ComponentTransformation.h"

#include "Primitive.h"

ComponentAnimation::ComponentAnimation(GameObject* parent, componentType type) : Component(parent, type)
{
}

ComponentAnimation::~ComponentAnimation()
{
}

bool ComponentAnimation::Update()
{
	bool ret = false;

	ResourceAnimation* animation = (ResourceAnimation*)App->resources->GetResource(RUID);
	if (animation != nullptr)
	{
		for (int i = 0; i < animation->numBones; i++)
		{
			GameObject* GO = gameObject->getChildByName(animation->bones[i].NodeName.c_str());
			if (GO != nullptr)
			{
				vec position = float3::zero;
				vec scale = float3::one;
				Quat rot = Quat::identity;
				if (animation->bones[i].numPosKeys > 0)
				{					
					position.x = animation->bones[i].PosKeysValues[0];
					position.y = animation->bones[i].PosKeysValues[1];
					position.z = animation->bones[i].PosKeysValues[2];
				}
				if (animation->bones[i].numScaleKeys > 0)
				{
					scale.x = animation->bones[i].ScaleKeysValues[0];
					scale.y = animation->bones[i].ScaleKeysValues[1];
					scale.z = animation->bones[i].ScaleKeysValues[2];
				}
				if (animation->bones[i].numRotKeys > 0)
				{
					rot.x = animation->bones[i].RotKeysValues[0];
					rot.y = animation->bones[i].RotKeysValues[1];
					rot.z = animation->bones[i].RotKeysValues[2];
					rot.w = animation->bones[i].RotKeysValues[3];
				}
				float4x4 mat;
				mat.Set(float4x4::FromTRS(position, rot, scale));

				ComponentTransformation* transform = (ComponentTransformation*)GO->GetComponent(TRANSFORMATION);
				mat = mat * transform->localMatrix;
				
				mat.Decompose(position, rot, scale);
				animation->spheres[i].SetPos(position.x, position.y, position.z);
				animation->spheres[i].Render();
			}
		}
	}

	return ret;
}

void ComponentAnimation::DrawInfo()
{
	ResourceAnimation* animation = (ResourceAnimation*)App->resources->GetResource(RUID);

	ImGui::PushID("toggleAnimation");
	ImGui::Checkbox("", &active);
	ImGui::PopID();
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
	{
		beginDroppableSpace((animation == nullptr) ? "No Animation" : animation->GetName(), animation == nullptr);
		ImGui::SameLine();
		pickResourceButton(R_ANIMATION);
		if (animation != nullptr)
		{
			ImGui::Text("Animation Times:\n Duration: %f | Speed: %f", animation->time,animation->ticksXsecond);
			ImGui::Text("Number of bones: %d", animation->numBones);
		}
	}
}

void ComponentAnimation::Save(JSON_Value * component) const
{
	ResourceAnimation* rAnimation = (ResourceAnimation*)App->resources->GetResource(RUID);

	JSON_Value* animation = component->createValue();

	animation->addInt("Type", type);
	animation->addUint("UID", UID);
	animation->addString("FBX", rAnimation->GetFile());
	animation->addString("animation", rAnimation->GetName());

	component->addValue("", animation);
}

void ComponentAnimation::Load(JSON_Value * component)
{
	RUID = App->resources->getResourceUIDFromMeta(component->getString("FBX"), "animations", component->getString("animation"));

	Resource* res = App->resources->GetResource(RUID);
	if (res != nullptr)
		res->LoadtoMemory();
}
