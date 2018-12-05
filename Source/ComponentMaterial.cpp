#include "Application.h"
#include "ComponentMaterial.h"
#include "ModuleTextures.h"
#include "ModuleResource.h"
#include "Resource.h"
#include "ResourceMaterial.h"
#include "ModuleFileSystem.h"

#ifdef _DEBUG
//#define TEST_MEMORY_MANAGER
#include "mmgr/mmgr.h"
#endif

ComponentMaterial::ComponentMaterial(GameObject * gameobject, componentType type) :Component(gameobject, type)
{
}

ComponentMaterial::~ComponentMaterial()
{
	ResourceMaterial* mat = (ResourceMaterial*)App->resources->GetResource(RUID);
	if (mat != nullptr)
		mat->UnloadMemory();
}

bool ComponentMaterial::Update(float dt)
{
	ResourceMaterial* mat = (ResourceMaterial*)App->resources->GetResource(RUID);
	if (!active || mat == nullptr)
		return false;

	if (mat->GetType() == R_COLOR)
	{
		glColor4f(mat->color.x, mat->color.y, mat->color.z, mat->color.w);
		if (transparentColor)
		{
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, 0.0f);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
	}
	else
	{
		//TMP
		glColor4f(1, 1, 1, 1);
		if (doAlphaTest)
		{
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, alphaTest);
		}
		if (doBlendColors)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		glBindTexture(GL_TEXTURE_2D, mat->GL_id);
	}

	return true;
}

void ComponentMaterial::DrawInfo()
{
	ResourceMaterial* mat = (ResourceMaterial*)App->resources->GetResource(RUID);

	ImGui::PushID("toggleMaterial");
	ImGui::Checkbox("", &active);
	ImGui::PopID();
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
	{
		beginDroppableSpace((mat == nullptr) ? "No Material" : mat->GetName(), mat == nullptr);
		// WIP
		/*if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE"))
			{
				std::string path = (const char*)payload->Data;
				path = path.substr(0, payload->DataSize); //For some reason, it reads more than data size, so cut it
				JSON_File* meta = App->resources->getMeta(path.c_str());
				UID = meta->getValue("meta")->getUint("UID");
				App->JSON_manager->closeFile(meta);
			}
			ImGui::EndDragDropTarget();
		}*/
		ImGui::SameLine();
		pickResourceButton(R_TEXTURE);

		if (mat != nullptr)
		{
			if (mat->GetType() == R_COLOR)
			{
				if (ImGui::RadioButton("Solid", !transparentColor))
					transparentColor = false;
				ImGui::SameLine();
				if (ImGui::RadioButton("Transparent", transparentColor))
					transparentColor = true;
				if (transparentColor)
				{
					float colors[4] = { mat->color.x, mat->color.y, mat->color.z , mat->color.w };
					if (ImGui::ColorPicker4("", colors, ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_HEX))
					{
						mat->color = { colors[0], colors[1], colors[2] , colors[3] };
					}
				}
				else
				{
					float colors[3] = { mat->color.x, mat->color.y, mat->color.z };
					if (ImGui::ColorPicker3("", colors, ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_HEX))
					{
						mat->color = { colors[0], colors[1], colors[2], mat->color.w };
					}
				}

			}
			else
			{

				ImGui::Text("Texture Size:\n Width: %d | Height: %d", mat->width, mat->height);
				float panelWidth = ImGui::GetWindowContentRegionWidth();
				if (panelWidth > 250)
					panelWidth = 250;
				float conversionFactor = panelWidth / mat->width;
				ImVec2 imageSize = { mat->height *conversionFactor, panelWidth };
				ImGui::Image((ImTextureID)mat->GL_id, imageSize);

				ImGui::Checkbox("Alpha Test", &doAlphaTest); ImGui::SameLine();
				ImGui::PushID("alphaTest slider");
				ImGui::SliderFloat("", &alphaTest, 0.0f, 1.0f);
				ImGui::PopID();
				ImGui::Checkbox("Blend Colors", &doBlendColors);
			}
		}
	}
}

void ComponentMaterial::Save(JSON_Value * component) const
{
	ResourceMaterial* mat = (ResourceMaterial*)App->resources->GetResource(RUID);

	JSON_Value* material = component->createValue();

	material->addInt("Type", type);
	material->addUint("UID", UID);
	material->addString("originalFile", mat->GetFile());

	component->addValue("", material);
}

void ComponentMaterial::Load(JSON_Value* component)
{
	//UID = component->getUint("UID");
	RUID = App->resources->getResourceUIDFromMeta(component->getString("originalFile"));		

	Resource* res = App->resources->GetResource(RUID);
	if (res != nullptr)
		res->LoadtoMemory();
}
