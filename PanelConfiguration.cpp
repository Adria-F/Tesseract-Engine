#include "Application.h"
#include "ModuleWindow.h"
#include "PanelConfiguration.h"
#include "ImGui\imgui.h"
#include "MathGeoLib/MathGeoLib.h"

PanelConfiguration::PanelConfiguration(const char * name) : Panel(name),
fps_log(FPS_LOG_SIZE), ms_log(FPS_LOG_SIZE)
{
	posX = 725;
	posY = 18;
	width = 354;
	height = 438;

	active = true;
}

PanelConfiguration::~PanelConfiguration()
{
}

void PanelConfiguration::Draw()
{
	ImGui::Begin(name.c_str(),&active);

	if (ImGui::CollapsingHeader("Application", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
	{

		static char app_name[120];
		strcpy_s(app_name, 120, App->getAppName());
		if (ImGui::InputText("App Name", app_name, 120, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			App->setAppName(app_name);

		static int framerateCap = App->getFramerateCap();
		if (ImGui::SliderInt("MaxFPS", &framerateCap, 1, 120))
			App->setFramerateCap(framerateCap);

		char title[25];
		sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
		ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
		sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
		ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
	}

	if (ImGui::CollapsingHeader("Window"))
	{
		if (ImGui::Checkbox("FullScreen", &fullscreen))
			windowConfig();

		ImGui::SameLine();
		if (ImGui::Checkbox("Resizable", &resizable))
			//SDL_SetWindowResizable()

		ImGui::NewLine();
		if (ImGui::Checkbox("Borderless", &borderless))
			SDL_SetWindowBordered(App->window->window, (SDL_bool)borderless);
		
		ImGui::SameLine();
		if (ImGui::Checkbox("Full Desktop", &full_desktop))
			windowConfig();
		
	}

	ImGui::End();
}

void PanelConfiguration::addFPS(float fps, float ms)
{
	for (uint i = 0; i < FPS_LOG_SIZE - 1; ++i)
	{
		fps_log[i] = fps_log[i + 1];
		ms_log[i] = ms_log[i + 1];
	}

	fps_log[FPS_LOG_SIZE - 1] = fps;
	ms_log[FPS_LOG_SIZE - 1] = ms;
}

void PanelConfiguration::windowConfig()
{
	Uint32 flags;

	if (fullscreen)
		SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN);
	else
		SDL_SetWindowFullscreen(App->window->window, 0);
	if (resizable)
		flags |= SDL_WINDOW_RESIZABLE;
	if (borderless)
		flags |= SDL_WINDOW_BORDERLESS;
	if (full_desktop)
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	
	
}
