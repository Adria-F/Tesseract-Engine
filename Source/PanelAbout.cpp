#include "PanelAbout.h"
#include "Application.h"
#include "DevIL\include\il.h"
#include "Assimp/include/version.h"

PanelAbout::PanelAbout(const char* name) : Panel(name)
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
	ImGui::Text("This engine is created as part of the 3D engines subject in the UPC - CITM.\n"
				"We attempt to create an engine with all the features needed in order to create the games\n"
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
	
	SDL_version version;
	SDL_GetVersion(&version);
	std::string sdlVersion = "SDL - version ";
	sdlVersion += std::to_string(version.major) + '.' + std::to_string(version.minor) + '.' + std::to_string(version.patch);
	if (ImGui::Selectable(sdlVersion.c_str(), false, 0, { 135, 13 }))
		App->RequestBrowser("https://www.libsdl.org/");
	
	std::string glVersion = (const char*)glGetString(GL_VERSION);
	glVersion = "OpenGL - version " + glVersion;
	if (ImGui::Selectable(glVersion.c_str(), false, 0, { 310, 13 }))
		App->RequestBrowser("https://www.opengl.org/");
	
	std::string imguiVersion = "ImGui - version ";
	imguiVersion +=  ImGui::GetVersion();
	if (ImGui::Selectable(imguiVersion.c_str(), false, 0, { 142, 13 }))
		App->RequestBrowser("https://github.com/ocornut/imgui");
	
	if (ImGui::Selectable("PCG - version (C Implementation) 0.94", false, 0, { 260, 13 }))
		App->RequestBrowser("http://www.pcg-random.org/");
	
	if (ImGui::Selectable("MathGeoLib - version 1.5.0", false, 0, { 185, 13 }))
		App->RequestBrowser("https://github.com/juj/MathGeoLib");
	
	std::string rapidjsonVersion = "RapidJSON - version ";
	rapidjsonVersion += RAPIDJSON_VERSION_STRING;
	if (ImGui::Selectable(rapidjsonVersion.c_str(), false, 0, { 175, 13 }))
		App->RequestBrowser("http://rapidjson.org/index.html");
	
	std::string glewVersion = (const char*)glewGetString(GLEW_VERSION);
	glewVersion = "Glew - version " + glewVersion;
	if (ImGui::Selectable(glewVersion.c_str(), false, 0, { 142, 13 }))
		App->RequestBrowser("https://github.com/nigels-com/glew");
	
	std::string assimpVersion = "Assimp - version ";
	assimpVersion += std::to_string(aiGetVersionMajor()) +'.'+ std::to_string(aiGetVersionMinor()) +'.'+ std::to_string(aiGetVersionRevision());
	
	if (ImGui::Selectable(assimpVersion.c_str(), false, 0, { 155, 13 }))
		App->RequestBrowser("http://www.assimp.org/");

	std::string devilVersion = "DevIL - version " + std::to_string(IL_VERSION);
	if (ImGui::Selectable(devilVersion.c_str(), false, 0, { 135, 13 }))
		App->RequestBrowser("http://openil.sourceforge.net/");

	//License
	ImGui::NewLine();
	ImGui::Text("This engine is licensed under the Public Domain, see LICENSE for more information");

	ImGui::End();
}
