#include "Application.h"
#include "ComponentAnimation.h"
#include "ModuleResource.h"
#include "Resource.h"
#include "ResourceAnimation.h"

#include "Component.h"

ComponentAnimation::ComponentAnimation(GameObject* parent, componentType type) : Component(parent, type)
{
}

ComponentAnimation::~ComponentAnimation()
{
}

bool ComponentAnimation::Update()
{
	bool ret = false;

	return ret;
}

void ComponentAnimation::DrawInfo()
{

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
	RUID = App->resources->getResourceUIDFromMeta(component->getString("FBX"), component->getString("animation"));

	Resource* res = App->resources->GetResource(RUID);
	if (res != nullptr)
		res->LoadtoMemory();
}
