#include "PanelFileDialog.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleWindow.h"
#include "ModuleScene.h"
#include "ModuleSceneLoader.h"

PanelFileDialog::PanelFileDialog(const char* name) : Panel(name)
{
}

PanelFileDialog::~PanelFileDialog()
{
	clearElements();
}

void PanelFileDialog::Draw()
{
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	ImGui::BeginMenuBar();
	ImGui::Text(path.c_str());
	ImGui::EndMenuBar();

	char text[120];
	strcpy_s(text, 120, App->scene_intro->scene_name.c_str());

	for (std::list<assetsElement*>::iterator it_e = elements.begin(); it_e != elements.end(); it_e++)
	{
		if (ImGui::Selectable((*it_e)->name.c_str())) //Need to check if it is a folder
			App->scene_intro->scene_name = (*it_e)->name;
	}

	ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 75);
	ImGui::Separator();
	ImGui::PushStyleColor(ImGuiCol_FrameBg, { 1.0f,1.0f,1.0f,0.9f });
	ImGui::PushStyleColor(ImGuiCol_Text, { 0.0f,0.0f,0.0f,1.0f });
	ImGui::SetCursorPosX(20);
	ImGui::PushItemWidth(ImGui::GetWindowWidth()-100);
	if (ImGui::InputText("", text, 120, ImGuiInputTextFlags_AutoSelectAll))
		App->scene_intro->scene_name = text;
	ImGui::PopItemWidth();
	ImGui::PopStyleColor(2);

	ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 35);
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 225);
	if (ImGui::Button(name.c_str(), { 100, 25 }))
	{
		if (name == "Save")
		{
			App->Save();
		}
		else
		{
			App->scene_loader->next_loadedScene_name = App->scene_intro->scene_name;
			App->Load();
		}
		active = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel", { 100, 25 }))
	{
		active = false;
	}

	ImGui::End();
}

void PanelFileDialog::SaveAt(const char * path, const char* exclusiveExtension)
{
	name = "Save";
	
	this->path = path;
	clearElements();
	App->fileSystem->getFilesAt(path, elements, exclusiveExtension);

	active = true;
}

void PanelFileDialog::LoadAt(const char * path, const char* exclusiveExtension)
{
	name = "Load";

	this->path = path;
	clearElements();
	App->fileSystem->getFilesAt(path, elements, exclusiveExtension);

	active = true;
}

void PanelFileDialog::clearElements()
{
	for (std::list<assetsElement*>::iterator it_e = elements.begin(); it_e != elements.end(); it_e++)
	{
		RELEASE(*it_e);
	}
	elements.clear();

	currElement = nullptr;
}
