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
	if (ImGui::Selectable("ImGui - version 1.65", false, 0, { 142, 13 }))
		App->RequestBrowser("https://github.com/ocornut/imgui");
	if (ImGui::Selectable("PCG - version (C Implementation) 0.94", false, 0, { 260, 13 }))
		App->RequestBrowser("http://www.pcg-random.org/");
	if (ImGui::Selectable("MathGeoLib", false, 0, { 70, 13 }))
		App->RequestBrowser("https://github.com/juj/MathGeoLib");
	if (ImGui::Selectable("RapidJSON - version 1.1.0", false, 0, { 175, 13 }))
		App->RequestBrowser("http://rapidjson.org/index.html");

	//License
	ImGui::NewLine();
	ImGui::Text("MIT License\n"
				"\n"
				"Copyright(c) 2018\n"
				"Adria Ferrer Comas & \n"
				"Marc Garcia Uceda\n"
				"\n"
				"Permission is hereby granted, free of charge,\n"
				"to any person obtaining a copy of this\n"
				"software and associated documentation files\n"
				"(the 'Software'), to deal in the Software\n"
				"without restriction, including without\n"
				"limitation the rights to use, copy, modify,\n"
				"merge, publish, distribute, sublicense,\n"
				"and/or sell copies of the Software, and to\n"
				"permit persons to whom the Software is\n"
				"furnished to do so, subject to the following\n"
				"conditions :\n"
				"\n"
				"The above copyright notice and this\n"
				"permission notice shall be included in all\n"
				"copies or substantial portions of the Software.\n"
				"\n"
				"THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT\n"
				"WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,\n"
				"INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF\n"
				"MERCHANTABILITY, FITNESS FOR A PARTICULAR\n"
				"PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL\n"
				"THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR\n"
				"ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER\n"
				"IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,\n"
				"ARISING FROM, OUT OF OR IN CONNECTION WITH\n"
				"THE SOFTWARE OR THE USE OR OTHER DEALINGS IN\n"
				"THE SOFTWARE."
	);

	ImGui::End();
}
