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

bool ComponentAnimation::Update(float dt)
{
	bool ret = false;

	ResourceAnimation* animation = (ResourceAnimation*)App->resources->GetResource(RUID);
	if (animation != nullptr && debugDraw)
	{
		for (int i = 0; i < animation->numBones; i++)
		{
			GameObject* GO = gameObject->getChildByName(animation->bones[i].NodeName.c_str());
			if (GO != nullptr)
			{
				animation->time += dt*0.005f;
				if (animation->time > animation->getDuration())
				{
					animation->time -= animation->getDuration();
				}

				if (animation->bones[i].calcCurrentIndex(animation->time*animation->ticksXsecond))
				{
					animation->bones[i].calcTransfrom();

					ComponentTransformation* transform = (ComponentTransformation*)GO->GetComponent(TRANSFORMATION);
					transform->localMatrix = animation->bones[i].lastTransform;

					vec position = float3::zero;
					vec scale = float3::one;
					Quat rot = Quat::identity;
					float4x4 mat = transform->globalMatrix;
					mat.Decompose(position, rot, scale);
					animation->spheres[i].SetPos(position.x, position.y, position.z);

					if (GO->parent != nullptr && GO->parent->GetComponent(ANIMATION) == nullptr)
					{
						animation->lines[i].origin = position;
						float4x4 parentMat = ((ComponentTransformation*)GO->parent->GetComponent(TRANSFORMATION))->globalMatrix;
						parentMat.Decompose(position, rot, scale);
						animation->lines[i].destination = position;
					}
				}

				animation->spheres[i].Render();
				animation->lines[i].Render();
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
			ImGui::Checkbox("Draw Bones", &debugDraw);
			ImGui::Text("Animation Times:\n Duration: %f | Speed: %f", animation->ticks,animation->ticksXsecond);
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
	animation->addBool("debugDraw", debugDraw);

	component->addValue("", animation);
}

void ComponentAnimation::Load(JSON_Value * component)
{
	RUID = App->resources->getResourceUIDFromMeta(component->getString("FBX"), "animations", component->getString("animation"));
	debugDraw = component->getBool("debugDraw");

	Resource* res = App->resources->GetResource(RUID);
	if (res != nullptr)
		res->LoadtoMemory();
}
