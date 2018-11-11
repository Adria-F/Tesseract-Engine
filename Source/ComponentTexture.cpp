#include "Application.h"
#include "ComponentTexture.h"
#include "ModuleTextures.h"
#include "Resource.h"
#include "ResourceTexture.h"


ComponentTexture::~ComponentTexture()
{
}

bool ComponentTexture::Update()
{
	if (!active || resource == nullptr)
		return false;

	if (resource != nullptr)
		glBindTexture(GL_TEXTURE_2D, resource->GL_id);
	else
		glColor3f(1, 1, 1);

	return true;
}

void ComponentTexture::DrawInfo()
{
	ImGui::PushID("toggleTexture");
	ImGui::Checkbox("", &active);
	ImGui::PopID();
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Texture", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
	{
		beginDroppableSpace((resource == nullptr) ? "No Texture" : resource->name.c_str(), resource ==nullptr);
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE", ImGuiDragDropFlags_AcceptBeforeDelivery))
			{

			}
			ImGui::EndDragDropTarget();
		}

		if (resource != nullptr)
		{
			ImGui::Text("Texture Size:\n Width: %d | Height: %d", resource->width, resource->height);
			float panelWidth = ImGui::GetWindowContentRegionWidth();
			if (panelWidth > 250)
				panelWidth = 250;
			float conversionFactor = panelWidth / resource->width;
			ImVec2 imageSize = { resource->height *conversionFactor, panelWidth };
			ImGui::Image((ImTextureID)resource->GL_id, imageSize);
		}
		else
		{
			ImGui::Text("No texture attached");
		}
	}
}

void ComponentTexture::Save(JSON_Value * component) const
{
	JSON_Value* texture = component->createValue();

	texture->addInt("Type", type);
	texture->addString("texture", resource->name.c_str());

	component->addValue("", texture);
}

void ComponentTexture::Load(JSON_Value * component)
{
	//TODO with resources
	Material = App->textures->loadTexture(component->getString("texture"));
}
