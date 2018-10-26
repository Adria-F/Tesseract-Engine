#include "PanelProperties.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"

PanelProperties::PanelProperties(const char* name): Panel(name)
{
	active = true;
}

PanelProperties::~PanelProperties()
{
}

void PanelProperties::Draw()
{
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_AlwaysVerticalScrollbar);	

	if (App->renderer3D->meshes.size() > 0)
	{
		ImGui::Text("Current loaded file has %d meshes", App->renderer3D->meshes.size());
		ImGui::NewLine();
		int count = 0;
		for (std::list<Mesh*>::iterator it_m = App->renderer3D->meshes.begin(); it_m != App->renderer3D->meshes.end(); it_m++)
		{
			Mesh* mesh = (*it_m);
			ImGui::Text("Mesh name: %s", mesh->name.c_str());

			ImGui::PushID("Transformation" + count);
			if (ImGui::CollapsingHeader("Transformation"))
			{
				ImGui::Text("Position:\n X: %f | Y: %f | Z: %f", mesh->position.x, mesh->position.y, mesh->position.z);
				float3 rot = mesh->rotation.ToEulerXYZ();
				rot *= 180/pi;
				ImGui::Text("Rotation:\n X: %f | Y: %f | Z: %f", rot.x, rot.y, rot.z);
				ImGui::Text("Scale:\n X: %f | Y: %f | Z: %f", mesh->scale.x, mesh->scale.y, mesh->scale.z);
			}
			ImGui::PopID();
			ImGui::PushID("Geometry" + count);
			if (ImGui::CollapsingHeader("Geometry"))
			{
				ImGui::Text("Triangles Count: %d", mesh->num_indices / 3);
				ImGui::Text("Vertices Count: %d", mesh->num_vertices);
				ImGui::Text("Mesh size:\n X: %f | Y: %f | Z: %f", mesh->boundingBox.Size().x, mesh->boundingBox.Size().y, mesh->boundingBox.Size().z);
			}
			ImGui::PopID();
			ImGui::PushID("Texture" + count);
			if (ImGui::CollapsingHeader("Texture"))
			{
				if (mesh->texture != 0)
				{
					ImGui::Text("Texture Size:\n Width: %d | Height: %d", mesh->texWidth, mesh->texHeight);
					float panelWidth = ImGui::GetWindowContentRegionWidth();
					float conversionFactor = panelWidth / mesh->texWidth;
					ImVec2 imageSize = { mesh->texHeight *conversionFactor, panelWidth };
					ImGui::Image((ImTextureID)mesh->texture, imageSize);
				}
				else
					ImGui::Text("No texture");
			}
			ImGui::PopID();

			count++;
			ImGui::NewLine();
		}
	}
	else
		ImGui::Text("No meshes loaded");

	ImGui::End();
}
