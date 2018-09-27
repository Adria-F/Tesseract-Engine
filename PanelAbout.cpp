#include "PanelAbout.h"
#include "Application.h"
#include "ImGui\imgui.h"

PanelAbout::PanelAbout(const char* name, float posX, float posY, float width, float height) : Panel(name, posX, posY, width, height)
{
}

PanelAbout::~PanelAbout()
{
}

void PanelAbout::Draw()
{
	ImGui::Begin(name.c_str(), &active);

	//Name and description
	ImGui::Text("Engine Name");
	ImGui::NewLine();
	ImGui::Text("This engine is created as part of the 3D engines\n"
				"subject in the UPC - CITM.\n"
				"We attempt to create an engine with all the\n"
				"features needed in order to create the games\n"
				"for our future projects."
	);

	//Authors
	ImGui::NewLine();
	ImGui::Text("Created by:");
	ImGui::NewLine(); ImGui::SameLine(80);
	if (ImGui::Selectable("Adria Ferrer", false, 0, { 85, 13 }))
		App->RequestBrowser("https://github.com/Adria-F");
	ImGui::SameLine(185);
	if (ImGui::Selectable("Max Garcia", false, 0, { 70, 13 }))
		App->RequestBrowser("https://github.com/MaxitoSama");

	//Libraries
	ImGui::NewLine();
	ImGui::Text("Libraries used:");
	if (ImGui::Selectable("SDL - version 2.0.8", false, 0, { 135, 13 }))
		App->RequestBrowser("https://www.libsdl.org/");
	if (ImGui::Selectable("ImGui - version %s", false, 0, { 142, 13 }))
		App->RequestBrowser("https://github.com/ocornut/imgui");
	if (ImGui::Selectable("PCG - version (C Implementation) 0.94", false, 0, { 260, 13 }))
		App->RequestBrowser("http://www.pcg-random.org/");
	if (ImGui::Selectable("MathGeoLib", false, 0, { 70, 13 }))
		App->RequestBrowser("https://github.com/juj/MathGeoLib");
	if (ImGui::Selectable("RapidJSON - version 1.1.0", false, 0, { 175, 13 }))
		App->RequestBrowser("http://rapidjson.org/index.html");
	if (ImGui::Selectable((const char*)("Glew - version %s", glewGetString(GLEW_VERSION)), false, 0, { 142, 13 }))
		App->RequestBrowser("https://github.com/nigels-com/glew");

	//License
	ImGui::NewLine();
	ImGui::Text("This engine is licensed under the Public\n"
				"Domain, see LICENSE for more information"

	);

	ImGui::End();
}
