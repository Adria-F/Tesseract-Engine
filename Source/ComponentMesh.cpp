#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleResource.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentTransformation.h"
#include "GameObject.h"
#include "ModuleMeshes.h"

#include "ModuleScene.h"
#include "ComponentBone.h"

#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceBone.h"

//#ifdef _DEBUG
////#define TEST_MEMORY_MANAGER
//#include "mmgr/mmgr.h"
//#endif

ComponentMesh::ComponentMesh(GameObject* parent, componentType type) : Component(parent, type)
{
	color = true;
}

ComponentMesh::~ComponentMesh()
{
	ResourceMesh* mesh = (ResourceMesh*)App->resources->GetResource(RUID);
	if (mesh != nullptr)
		mesh->UnloadMemory();
}

bool ComponentMesh::Update(float dt)
{

	return true;
}

void ComponentMesh::DrawInfo()
{
	ResourceMesh* mesh = (ResourceMesh*)App->resources->GetResource(RUID);

	ImGui::PushID("toggleMesh");
	ImGui::Checkbox("", &active);
	ImGui::PopID();
	ImGui::SameLine();
	
	if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
	{
		beginDroppableSpace((mesh == nullptr) ? "No Mesh" : mesh->GetName(), mesh == nullptr);
		ImGui::SameLine();
		pickResourceButton(R_MESH);
		if (mesh != nullptr)
		{
			ImGui::Text("Triangles Count: %d", mesh->num_indices / 3);
			ImGui::Text("Vertices Count: %d", mesh->num_vertices);
			ImGui::Text("Mesh size:\n X: %f | Y: %f | Z: %f", mesh->boundingBox.Size().x, mesh->boundingBox.Size().y, mesh->boundingBox.Size().z);
			ImGui::Text("Bone Count: %d", mesh->numBones);

			if (ImGui::TreeNodeEx("Using Bones"))
			{
				for (int i = 0; i < componentsBones.size(); i++)
				{
					Component* bone = App->scene_intro->getComponent(componentsBones[i]);
					if (bone != nullptr)
					{
						Resource* rBone = App->resources->GetResource(bone->RUID);
						if (rBone != nullptr)
						{
							ImGui::Text(rBone->GetName());
						}
					}
				}
				ImGui::TreePop();
			}
		}
	}
}

float* ComponentMesh::Skining()
{
	ResourceMesh* mesh = (ResourceMesh*)App->resources->GetResource(RUID);
	if (mesh != nullptr && componentsBones.size() > 0 && gameObject->parent != nullptr && gameObject->parent->GetComponent(ANIMATION) != nullptr)
	{
		float* vertices = new float[mesh->num_vertices * 3];
		memset(vertices, 0, sizeof(float)*mesh->num_vertices * 3);

		bool hasBones = false;
		for (int i = 0; i < componentsBones.size(); i++)
		{
			ComponentBone* bone = (ComponentBone*)App->scene_intro->getComponent(componentsBones[i]);
			ResourceBone* rBone = (ResourceBone*)App->resources->GetResource(bone->RUID);
			if (bone != nullptr && rBone != nullptr)
			{				
				hasBones = true;
				float4x4 boneTransform = (((ComponentTransformation*)gameObject->GetComponent(TRANSFORMATION))->globalMatrix.Inverted()*((ComponentTransformation*)bone->gameObject->GetComponent(TRANSFORMATION))->globalMatrix)*rBone->Offset;

				for (int j = 0; j < rBone->numWeights; j++)
				{
					uint VertexIndex = rBone->weights[j].VertexID;
					if (VertexIndex*3 >= mesh->num_vertices * 3)
						continue;
					float3 startingVertex(&mesh->vertices[VertexIndex * 3]);
					float3 movementWeight = boneTransform.TransformPos(startingVertex);

					vertices[VertexIndex * 3] += movementWeight.x*rBone->weights[j].weight;
					vertices[VertexIndex * 3 + 1] += movementWeight.y*rBone->weights[j].weight;
					vertices[VertexIndex * 3 + 2] += movementWeight.z*rBone->weights[j].weight;
				}
			}
		}

		if (!hasBones)
			return nullptr;
		else
			return vertices;
	}

	return nullptr;
}

void ComponentMesh::Save(JSON_Value * component) const
{
	ResourceMesh* rMesh = (ResourceMesh*)App->resources->GetResource(RUID);

	JSON_Value* mesh = component->createValue();

	mesh->addInt("Type", type);
	mesh->addUint("UID",UID);
	
	if (rMesh != nullptr)
	{
		mesh->addString("FBX", rMesh->GetFile());
		mesh->addString("mesh", rMesh->GetName());
	}

	JSON_Value* linker = mesh->createValue();

	linker->convertToArray();

	for (int i = 0; i < componentsBones.size(); i++)
	{
		linker->addUint("",componentsBones[i]);
	}

	mesh->addValue("Linker", linker);

	component->addValue("", mesh);
}

void ComponentMesh::Load(JSON_Value* component)
{
	//UID = component->getUint("UID");
	RUID = App->resources->getResourceUIDFromMeta(component->getString("FBX"), "meshes", component->getString("mesh"));

	JSON_Value* linker = component->getValue("Linker");

	if (linker != nullptr)
	{
		if (linker->getRapidJSONValue()->IsArray())
		{
			for (int i = 0; i < linker->getRapidJSONValue()->Size(); i++)
			{
				JSON_Value* boneUID = linker->getValueFromArray(i);
				uint value = boneUID->getRapidJSONValue()->GetUint();

				componentsBones.push_back(value);
			}
		}
	}

	Resource* res = App->resources->GetResource(RUID);
	if (res != nullptr)
		res->LoadtoMemory();

}
