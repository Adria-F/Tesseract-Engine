#include "PanelAssets.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ModuleSceneLoader.h"

#include "ImGui\imgui_internal.h"

PanelAssets::PanelAssets(const char* name) : Panel(name)
{
	active = true;

	current_path = ASSETS_FOLDER;
	App->fileSystem->getFilesAt(ASSETS_FOLDER, elements, nullptr, "meta");
}

PanelAssets::~PanelAssets()
{
	clearElements();
}

void PanelAssets::Draw()
{
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_MenuBar);

	float folderWidth = (float)App->gui->folder->width;

	//Buttons
	ImGui::BeginMenuBar();
	ImGui::PushStyleColor(ImGuiCol_Button, { 0.9f,0.45f,0.0f,0.7f });
	if (ImGui::ArrowButton("back", ImGuiDir_Left))
	{
		clearElements();
		App->fileSystem->splitPath(current_path.c_str(), &current_path, nullptr, nullptr);
		App->fileSystem->getFilesAt(current_path.c_str(), elements, nullptr, "meta");
	}
	ImGui::Text(current_path.c_str());
	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 40);
	if (ImGui::Button("Reload"))
	{
		clearElements();
		current_path = ASSETS_FOLDER;
		App->fileSystem->getFilesAt(ASSETS_FOLDER, elements, nullptr, "meta");
	}
	ImGui::PopStyleColor();
	ImGui::EndMenuBar();

	//Files
	ImGui::PushStyleColor(ImGuiCol_Button, { 1.0f,1.0f,1.0f,0.2f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f,0.37f,0.0f,0.7f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.95f,0.5f,0.0f,0.7f });
	ImGui::PushStyleColor(ImGuiCol_Header, { 0.0f,0.0f,0.0f,0.0f });
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, { 0.0f,0.0f,0.0f,0.0f });
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, { 0.0f,0.0f,0.0f,0.0f });

	int i = 0;
	int rowCount = 1;
	float leftMargin = ImGui::GetCursorPosX();
	float currLine = ImGui::GetCursorPosY();
	for (std::list<assetsElement*>::iterator it_e = elements.begin(); it_e != elements.end(); it_e++)
	{
		if ((*it_e)->selected)
			ImGui::PushStyleColor(ImGuiCol_Button, { 0.95f,0.5f,0.0f,0.7f });
		else
			ImGui::PushStyleColor(ImGuiCol_Button, { 1.0f,1.0f,1.0f,0.2f });

		//Set proper cursor position
		if (rowCount == 1)
			currLine = ImGui::GetCursorPosY();
		else
		{
			if (ImGui::GetWindowContentRegionMax().x >= 70 * rowCount)
			{
				ImGui::SetCursorPos({ (float)leftMargin + (50 + 30)*(rowCount - 1), currLine });
			}
			else
			{
				rowCount = 1;
				currLine = ImGui::GetCursorPosY();
			}
		}
		
		ImGui::PushID(i);
		if (ImGui::ImageButton((ImTextureID)(((*it_e)->type == assetsElement::FOLDER) ? App->gui->folder->GL_id : App->gui->file->GL_id), { 50, 50 }))
		{
			selectElement((*it_e));
			if ((*it_e)->type == assetsElement::FILE)
			{
				//Show import setting on inspector
			}
		}
		ImGui::PopID();
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) //Import asset if double clicked
		{
			if ((*it_e)->type == assetsElement::FOLDER)
			{
				if (current_path.size() > 0 && current_path.back() != '/')
					current_path += '/';
				current_path += (*it_e)->name;
				clearElements();
				App->fileSystem->getFilesAt(current_path.c_str(), elements, nullptr, "meta");
				ImGui::PopStyleColor();
				break;
			}
			else
			{
				std::string extension;
				std::string filename;
				App->fileSystem->splitPath((*it_e)->name.c_str(), nullptr, &filename, &extension);

				if (extension == "fbx" || extension == "FBX")
				{
					App->scene_loader->loadScene(filename.c_str(), true);
				}
			}
		}
		if (ImGui::BeginDragDropSource()) //Drag source for resources
		{
			std::string path = current_path;
			if (path.size() > 0 && path.back() != '/')
				path += '/';
			path += (*it_e)->name;			
			ImGui::SetDragDropPayload("TEXTURE", path.c_str(), sizeof(char)*path.size());
			ImGui::Text((*it_e)->name.c_str());
			ImGui::EndDragDropSource();
		}

		//Draw name
		if (rowCount > 1 && ImGui::GetWindowContentRegionMax().x >= 70 * rowCount)
		{
			ImGui::SetCursorPosX((float)leftMargin + (50 + 30)*(rowCount - 1));
		}
		/*if ((*it_e)->renaming)  FILE RENAMING - WIP
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, { 1.0f,1.0f,1.0f,0.9f });
			ImGui::PushStyleColor(ImGuiCol_Text, { 0.0f,0.0f,0.0f,1.0f });
			ImGui::PushItemWidth(60);
			char text[120];
			strcpy_s(text, 120, (*it_e)->name.c_str());	
			ImGui::PushID(-i);
			if (ImGui::InputText("", text, 120, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
			{
				std::string path = current_path;
				if (path.size() > 0 && path.back() != '/')
					path += '/';
				path += (*it_e)->name;
				std::string extension;
				App->fileSystem->splitPath((*it_e)->name.c_str(), nullptr, nullptr, &extension);
				(*it_e)->name = text;
				if (extension.size() > 0)
					(*it_e)->name += '.' + extension;
				App->fileSystem->renameFile(path.c_str(), text);
				(*it_e)->renaming = false;
			}
			ImGui::PopID();
			ImGui::PopItemWidth();
			ImGui::PopStyleColor(2);
		}
		else*/
			ImGui::Selectable((*it_e)->name.c_str(), false, 0, { 60, 13 });

		rowCount++;
		i++;

		ImGui::PopStyleColor();
	}

	ImGui::PopStyleColor(6);

	ImGui::End();
}

void PanelAssets::pushElement(assetsElement* element)
{
	elements.push_back(element);
}

void PanelAssets::clearElements()
{
	for (std::list<assetsElement*>::iterator it_e = elements.begin(); it_e != elements.end(); it_e++)
	{
		RELEASE(*it_e);
	}
	elements.clear();
}

void PanelAssets::selectElement(assetsElement* element)
{
	if (selected_element != nullptr)
		selected_element->selected = false;

	selected_element = element;
	selected_element->selected = true;
}
