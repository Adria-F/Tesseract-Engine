#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleGUI.h"
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
	smoothT = true;
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
		if(!TestPause)
			animTime += dt*speed;
		
		if (animTime > animation->getDuration() && loop)
		{
			animTime -= animation->getDuration();
		}

		for (int i = 0; i < animation->numBones; i++)
		{
			if (bones.find(i) == bones.end()) //If no game object assigned
				continue;

			GameObject* GO = App->scene_intro->getGameObject(bones[i]);
			if (GO != nullptr)
			{
				ComponentTransformation* transform = (ComponentTransformation*)GO->GetComponent(TRANSFORMATION);
				if (!blend || !smoothT)
				{
					if (animation->boneTransformations[i].calcCurrentIndex(animTime*animation->ticksXsecond,TestPlay))
					{
						animation->boneTransformations[i].calcTransfrom(animTime*animation->ticksXsecond);
						transform->localMatrix = animation->boneTransformations[i].lastTransform;
					}
				}
				else 
				{
					if (blendBones.find(i) == blendBones.end()) //If no game object assigned
						continue;

					ResourceAnimation* rblendAnimation = (ResourceAnimation*)App->resources->GetResource(blendRUID);
					
					if (rblendAnimation != nullptr)
					{
						if (blendAnimTime > rblendAnimation->getDuration() && loop)
						{
							blendAnimTime -= rblendAnimation->getDuration();
						}

						animation->boneTransformations[i].calcCurrentIndex(animTime*animation->ticksXsecond, TestPlay);
						rblendAnimation->boneTransformations[i].calcCurrentIndex(blendAnimTime*rblendAnimation->ticksXsecond, TestPlay);
						
						animation->boneTransformations[i].calcTransfrom(animTime*animation->ticksXsecond);
						rblendAnimation->boneTransformations[i].calcTransfrom(blendAnimTime*rblendAnimation->ticksXsecond);
						
						animation->boneTransformations[i].smoothBlending(rblendAnimation->boneTransformations[i].lastTransform, blendTime / totalBlendTime);
						transform->localMatrix = animation->boneTransformations[i].lastTransform;
					}
				}
			}
		}

		if (blend && smoothT)
		{
			blendTime += dt * speed;
			blendAnimTime += dt * speed;

			if (blendTime > totalBlendTime)
			{
				assignResource(blendRUID);
				animTime = blendAnimTime;
				blendAnimTime = 0.0f;
				blendTime = 0.0f;
				blendRUID = 0.0f;
				blend = false;
				loop = blendLoop;
			}
		}
	}

	return ret;
}

void ComponentAnimation::DrawInfo()
{
	ResourceAnimation* animation = (ResourceAnimation*)App->resources->GetResource(RUID);
	ResourceAnimation* blendAnimation = (ResourceAnimation*)App->resources->GetResource(blendRUID);

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

			ImGui::Text("Animation Speed:");
			ImGui::SameLine();

			ImGui::PushID("Speed");
			ImGui::InputFloat("", &speed, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopID();

			ImGui::Text("Animation Times:\nDuration: %.2f | Speed: %f", animation->ticks,animation->ticksXsecond);
			ImGui::Text("Number of bones: %d", animation->numBones);
		}

		ImGui::NewLine();

		ImGui::Text("Blend Animation");

		ImGui::Text("Blend time:");
		ImGui::SameLine();
		
		ImGui::PushID("total blend time");
		ImGui::InputFloat("", &totalBlendTime, 0.0f, 0.0f,"%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::PopID();

		if (ImGui::Checkbox("Smooth Transition", &smoothT))
		{
			frozenT = false;
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

void ComponentAnimation::assignResource(uint UID, bool doBlend, bool blendloop)
{
	if (!doBlend)
	{
		Component::assignResource(UID);
		bones.clear();
		animTime = 0;
	}
	else
	{
		blendRUID = UID;
		blendBones.clear();
		blend = true;
		blendTime = 0.0f;
		blendAnimTime = 0.0f;
		blendLoop = blendloop;
	}

	ResourceAnimation* animation = (ResourceAnimation*)App->resources->GetResource(UID);
	if (animation != nullptr)
	{
		if (doBlend)
		{
			animation->LoadtoMemory();
		}

		for (int i = 0; i < animation->numBones; i++)
		{
			GameObject* GO = gameObject->getChildByName(animation->boneTransformations[i].NodeName.c_str());
			if (GO != nullptr)
			{
				if (!doBlend)
					bones[i] = GO->UID;
				else
					blendBones[i] = GO->UID;
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
	animation->addBool("loop", loop);

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
	loop = component->getBool("loop");

	Resource* res = App->resources->GetResource(RUID);
	if (res != nullptr)
		res->LoadtoMemory();
}

bool ComponentAnimation::Finished() const
{
	ResourceAnimation* animation = (ResourceAnimation*)App->resources->GetResource(RUID);
	if (animation != nullptr)
	{
		if (animTime > animation->getDuration() && !loop)
			return true;
	}

	return false;
}


