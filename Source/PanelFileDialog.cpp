#include "PanelFileDialog.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleWindow.h"
#include "ModuleScene.h"
#include "ModuleSceneLoader.h"
#include "ModuleGUI.h"
#include "ModuleTextures.h"

#ifdef _DEBUG
//#define TEST_MEMORY_MANAGER
#include "mmgr/mmgr.h"
#endif

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
	if (ImGui::ArrowButton("back", ImGuiDir_Left))
	{
		clearElements();
		App->fileSystem->splitPath(current_path.c_str(), &current_path, nullptr, nullptr);
		if (current_path.size() == 0)
			current_path = ASSETS_FOLDER;
		if (current_path.back() == '/')
			current_path.pop_back();
		App->fileSystem->getFilesAt(current_path.c_str(), elements, (exclusive_extension.size() > 0) ? exclusive_extension.c_str() : nullptr);
	} 
	ImGui::SameLine();
	ImGui::Text(current_path.c_str());
	ImGui::EndMenuBar();

	char text[120];
	strcpy_s(text, 120, App->scene_intro->scene_name.c_str());

	for (std::list<assetsElement*>::iterator it_e = elements.begin(); it_e != elements.end(); it_e++)
	{
		if ((*it_e)->type == assetsElement::FOLDER)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, { 1.0f,1.0f,1.0f,0.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f,0.37f,0.0f,0.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.95f,0.5f,0.0f,0.0f });
			ImGui::ImageButton((ImTextureID)(App->gui->small_folder->GL_id), { 12, 12 });
			ImGui::PopStyleColor(3);
			ImGui::SameLine();
			ImGui::SetCursorPos({ ImGui::GetCursorPosX() - 2, ImGui::GetCursorPosY() + 2 });
		}
		if (ImGui::Selectable((*it_e)->name.c_str(), (*it_e)->selected)) //Need to check if it is a folder
		{
			if (selected_element != nullptr)
				selected_element->selected = false;
			selected_element = (*it_e);
			(*it_e)->selected = true;
			if ((*it_e)->type == assetsElement::FILE)
				App->scene_intro->scene_name = (*it_e)->name;
		}
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
		{
			if ((*it_e)->type == assetsElement::FOLDER)
			{
				if (current_path.size() > 0 && current_path.back() != '/')
					current_path += '/';
				current_path += (*it_e)->name;
				clearElements();
				App->fileSystem->getFilesAt(current_path.c_str(), elements, (exclusive_extension.size() > 0)? exclusive_extension.c_str():nullptr);
				break;
			}
			else
			{
				App->scene_intro->scene_name = (*it_e)->name;
				completed = true;
			}
		}
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
		completed = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel", { 100, 25 }))
	{
		active = false;
	}

	if (completed)
	{
		if (name == "Save")
		{
			App->scene_loader->next_scene_path = current_path + '/' + App->scene_intro->scene_name;
			App->Save();
		}
		else
		{
			App->scene_loader->next_scene_path = current_path + '/' + App->scene_intro->scene_name;
			App->Load();
		}
		active = false;
		completed = false;
	}

	ImGui::End();
}

void PanelFileDialog::SaveAt(const char * path, const char* exclusiveExtension)
{
	name = "Save";
	
	this->current_path = path;
	if (current_path.size() > 0 && current_path.back() == '/')
		current_path.pop_back();
	exclusive_extension = exclusiveExtension;
	clearElements();
	App->fileSystem->getFilesAt(path, elements, exclusiveExtension);

	active = true;
}

void PanelFileDialog::LoadAt(const char * path, const char* exclusiveExtension)
{
	name = "Load";

	this->current_path = path;
	if (current_path.size() > 0 && current_path.back() == '/')
		current_path.pop_back();
	exclusive_extension = exclusiveExtension;
	clearElements();
	App->fileSystem->getFilesAt(path, elements, exclusiveExtension);

	active = true;
}

void PanelFileDialog::clearElements()
{
	std::list<assetsElement*>::iterator it_e;
	it_e = elements.begin();
	while (it_e != elements.end())
	{
		RELEASE(*it_e);
		it_e++;
	}
	elements.clear();
}

void PanelFileDialog::selectElement(assetsElement* element)
{
	if (selected_element != nullptr)
		selected_element->selected = false;

	selected_element = element;
	selected_element->selected = true;
}