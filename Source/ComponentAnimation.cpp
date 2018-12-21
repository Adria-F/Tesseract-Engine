#include "Application.h"
#include "ComponentAnimation.h"
#include "ModuleResource.h"
#include "Resource.h"
#include "ResourceAnimation.h"
#include "GameObject.h"
#include "ModuleScene.h"

#include "Component.h"
#include "ComponentTransformation.h"
#include "ComponentBone.h"

#include "ResourceBone.h"

ComponentAnimation::ComponentAnimation(GameObject* parent, componentType type) : Component(parent, type)
{
}

ComponentAnimation::~ComponentAnimation()
{
}

bool ComponentAnimation::Update(float dt)
{
	bool ret = false;

	if (!bonesLoaded)
	{
		assignResource(RUID); //To assign used bones
		bonesLoaded = true;
	}

	ResourceAnimation* animation = (ResourceAnimation*)App->resources->GetResource(RUID);
	if (animation != nullptr)
	{
		for (int i = 0; i < animation->numBones; i++)
		{
			if (bones.find(i) == bones.end()) //If no game object assigned
				continue;

			GameObject* GO = App->scene_intro->getGameObject(bones[i]);
			if (GO != nullptr)
			{
				animation->time += dt*0.02f;
				if (animation->time > animation->getDuration() && loop)
				{
					animation->time -= animation->getDuration();
				}

				if (animation->boneTransformations[i].calcCurrentIndex(animation->time*animation->ticksXsecond))
				{
					animation->boneTransformations[i].calcTransfrom(animation->time*animation->ticksXsecond);

					ComponentTransformation* transform = (ComponentTransformation*)GO->GetComponent(TRANSFORMATION);
					transform->localMatrix = animation->boneTransformations[i].lastTransform;
				}
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
			ImGui::Checkbox("Loop", &loop);

			if (ImGui::Checkbox("Draw Bones", &debugDraw))
			{
				activateDebugBones(gameObject, debugDraw);
			}
			ImGui::Text("Animation Times:\n Duration: %f | Speed: %f", animation->ticks,animation->ticksXsecond);
			ImGui::Text("Number of bones: %d", animation->numBones);
		}

	}
}

void ComponentAnimation::activateDebugBones(GameObject* GO, bool active)
{
	ComponentBone* bone = (ComponentBone*)GO->GetComponent(BONE);
	if (bone != nullptr)
		bone->debugDraw = active;

	for (std::list<GameObject*>::iterator it_go = GO->childs.begin(); it_go != GO->childs.end(); it_go++)
	{
		activateDebugBones((*it_go), active);
	}
}

void ComponentAnimation::assignResource(uint UID)
{
	Component::assignResource(UID);
	bones.clear();

	ResourceAnimation* animation = (ResourceAnimation*)App->resources->GetResource(RUID);
	if (animation != nullptr)
	{
		animation->time = 0;

		for (int i = 0; i < animation->numBones; i++)
		{
			GameObject* GO = gameObject->getChildByName(animation->boneTransformations[i].NodeName.c_str());
			if (GO != nullptr)
			{
				bones[i] = GO->UID;
			}
		}
	}
}

void ComponentAnimation::Save(JSON_Value * component) const
{
	ResourceAnimation* rAnimation = (ResourceAnimation*)App->resources->GetResource(RUID);

	JSON_Value* animation = component->createValue();

	animation->addInt("Type", type);
	animation->addUint("UID", UID);
	animation->addBool("debugDraw", debugDraw);

	if (rAnimation != nullptr)
	{
		animation->addString("FBX", rAnimation->GetFile());
		animation->addString("animation", rAnimation->GetName());
	}

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

bool ComponentAnimation::Finished() const
{
	ResourceAnimation* animation = (ResourceAnimation*)App->resources->GetResource(RUID);
	if (animation != nullptr)
	{
		if (animation->time > animation->getDuration() && !loop)
			return true;
	}

	return false;
}
