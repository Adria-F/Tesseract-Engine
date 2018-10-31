#include "Application.h"
#include "ComponentTexture.h"
#include "ModuleTextures.h"


ComponentTexture::~ComponentTexture()
{
}

bool ComponentTexture::Update()
{
	if (!active)
		return false;

	if (Material != nullptr)
		glBindTexture(GL_TEXTURE_2D, Material->GL_id);
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
		if (Material != nullptr)
		{
			ImGui::Text("Texture Size:\n Width: %d | Height: %d", Material->width, Material->height);
			float panelWidth = ImGui::GetWindowContentRegionWidth();
			if (panelWidth > 250)
				panelWidth = 250;
			float conversionFactor = panelWidth / Material->width;
			ImVec2 imageSize = { Material->height *conversionFactor, panelWidth };
			ImGui::Image((ImTextureID)Material->GL_id, imageSize);
		}
		else
			ImGui::Text("No texture");
	}
}
