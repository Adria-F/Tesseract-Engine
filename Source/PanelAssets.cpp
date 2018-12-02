#include "PanelAssets.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ModuleSceneLoader.h"
#include "ModuleResource.h"

#include "ImGui\imgui_internal.h"

#ifdef _DEBUG
//#define TEST_MEMORY_MANAGER
#include "mmgr/mmgr.h"
#endif

PanelAssets::PanelAssets(const char* name) : Panel(name)
{
	active = true;

	current_path = ASSETS_FOLDER;
	current_path.pop_back();
	App->fileSystem->getFilesAt(ASSETS_FOLDER, elements, nullptr, "meta");
	update_delay = 1000;
}

PanelAssets::~PanelAssets()
{
	clearElements();
}

void PanelAssets::Draw()
{
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_MenuBar);

	if (force_update || (!renaming && update_timer.ReadTime() >= update_delay))
	{
		clearElements(); //BUG: Possible crash? NOTIFY (When happens when loading an scene?)
		App->fileSystem->getFilesAt(current_path.c_str(), elements, nullptr, "meta");
		update_timer.Start();
	}

	float folderWidth = (float)App->gui->folder->width;

	//Buttons
	ImGui::BeginMenuBar();
	ImGui::PushStyleColor(ImGuiCol_Button, { 0.9f,0.45f,0.0f,0.7f });
	if (ImGui::ArrowButton("back", ImGuiDir_Left))
	{
		clearElements();
		App->fileSystem->splitPath(current_path.c_str(), &current_path, nullptr, nullptr);
		if (current_path.size() == 0)
			current_path = ASSETS_FOLDER;
		if (current_path.back() == '/')
			current_path.pop_back();
		App->fileSystem->getFilesAt(current_path.c_str(), elements, nullptr, "meta");
	}
	std::string prevPath;
	std::string currDir;
	App->fileSystem->splitPath(current_path.c_str(), &prevPath, &currDir, nullptr);
	ImGui::Text(prevPath.c_str());
	if (prevPath.size() > 0 && ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE"))
		{
			std::string path = (const char*)payload->Data;
			path = path.substr(0, payload->DataSize); //For some reason, it reads more than data size, so cut it
			std::string curr_path = current_path;
			App->fileSystem->copyFile(path.c_str(), prevPath.c_str(), true);
			force_update = true;
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::SameLine(ImGui::GetCursorPosX()-13); ImGui::Text(currDir.c_str());
	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 40);
	ImGui::Button("Create");
	if (creating)
	{
		if (ImGui::BeginPopupContextItem("create", 0))
		{
			if (ImGui::Button("Folder"))
			{
				std::string path = current_path;
				if (path.size() > 0 && path.back() != '/')
					path += '/';
				path += "Unnamed";
				uint num_version = App->fileSystem->getAvailablePath(path.c_str(), &path);
				App->fileSystem->createDirectory(path.c_str());

				assetsElement* newFolder = new assetsElement();
				newFolder->type == assetsElement::FOLDER;
				newFolder->renaming = true;
				newFolder->name = "Unnamed";
				if (num_version > 0)
					newFolder->name += '(' + std::to_string(num_version) + ')';
				elements.push_back(newFolder);

				creating = false;
				renaming = true;
			}
			ImGui::EndPopup();
		}
	}
	else
		creating = true;

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

				if (extension == "fbx" || extension == "FBX" || extension == "dae")
				{
					std::string path = current_path;
					if (path.size() > 0 && path.back() != '/')
						path += '/';
					path += (*it_e)->name;
					uint UID = App->resources->getResourceUIDFromMeta(path.c_str());
					App->scene_loader->loadSceneFile(std::to_string(UID).c_str(), true);
				}
			}
		}
		if (ImGui::BeginPopupContextItem(((*it_e)->name + "rightClick").c_str(), 1))
		{
			if (ImGui::Button("Delete"))
			{
				std::string path = current_path;
				if (path.size() > 0 && path.back() != '/')
					path += '/';
				path += (*it_e)->name;
				App->fileSystem->deleteFile(path.c_str());
				
				assetsElement* element = (*it_e);
				it_e--; //What if is the only file
				elements.remove(element);
				RELEASE(element);
			}
			ImGui::EndPopup();
		}
		if ((*it_e)->type == assetsElement::FILE)
		{
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

			if (ImGui::BeginDragDropSource()) //Drag source for moving files between folders
			{
				std::string path = current_path;
				if (path.size() > 0 && path.back() != '/')
					path += '/';
				path += (*it_e)->name;
				ImGui::SetDragDropPayload("FILE", path.c_str(), sizeof(char)*path.size());
				ImGui::Text((*it_e)->name.c_str());
				ImGui::EndDragDropSource();
			}
		}
		else
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE"))
				{
					std::string path = (const char*)payload->Data;
					path = path.substr(0, payload->DataSize); //For some reason, it reads more than data size, so cut it
					std::string curr_path = current_path;
					if (curr_path.size() > 0 && curr_path.back() != '/')
						curr_path += '/';
					curr_path += (*it_e)->name;
					App->fileSystem->copyFile(path.c_str(), curr_path.c_str(), true);
					force_update = true;
				}
				ImGui::EndDragDropTarget();
			}
		}

		//Draw name
		if (rowCount > 1 && ImGui::GetWindowContentRegionMax().x >= 70 * rowCount)
		{
			ImGui::SetCursorPosX((float)leftMargin + (50 + 30)*(rowCount - 1));
		}
		if ((*it_e)->renaming)
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, { 1.0f,1.0f,1.0f,0.9f });
			ImGui::PushStyleColor(ImGuiCol_Text, { 0.0f,0.0f,0.0f,1.0f });
			ImGui::PushItemWidth(60);
			char text[120];
			strcpy_s(text, 120, (*it_e)->name.c_str());
			ImGui::PushID(-i);
			if (ImGui::InputText("", text, 120, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue) || (!ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)))
			{
				if ((*it_e)->name != text)
				{
					std::string path = current_path;
					if (path.size() > 0 && path.back() != '/')
						path += '/';
					path += (*it_e)->name;

					std::string extension;
					App->fileSystem->splitPath((*it_e)->name.c_str(), nullptr, nullptr, &extension);
					(*it_e)->name = text;
					if ((*it_e)->name.size() == 0)
						(*it_e)->name = "Unnamed";
					if (extension.size() > 0)
						(*it_e)->name += '.' + extension;
					std::string filePath;
					App->fileSystem->splitPath(path.c_str(), &filePath, nullptr, nullptr);
					uint num_version = App->fileSystem->getAvailablePath((filePath+(*it_e)->name).c_str(), nullptr);
					if (num_version > 0)
						(*it_e)->name += '(' + std::to_string(num_version) + ')';
					App->fileSystem->renameFile(path.c_str(), text);
				}
				(*it_e)->renaming = false;
				renaming = false;
			}
			if (!ImGui::IsItemFocused())
				ImGui::SetKeyboardFocusHere(0);
			ImGui::PopID();
			ImGui::PopItemWidth();
			ImGui::PopStyleColor(2);
		}
		else
			ImGui::Selectable((*it_e)->name.c_str(), false, 0, { 60, 13 });

		/*if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
		{			
			(*it_e)->renaming = true;
		}*/

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
	std::list<assetsElement*>::iterator it_e;
	it_e = elements.begin();
	while (it_e != elements.end())
	{
		RELEASE(*it_e);
		it_e++;
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
