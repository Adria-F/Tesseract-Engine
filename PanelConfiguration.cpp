#include "PanelConfiguration.h"
#include "Application.h"
#include "ImGui\imgui.h"
#include "MathGeoLib/MathGeoLib.h"

PanelConfiguration::PanelConfiguration(const char * name) : Panel(name)
{
	posX = 725;
	posY = 18;
	width = 354;
	height = 438;
}

PanelConfiguration::~PanelConfiguration()
{
}

void PanelConfiguration::Draw()
{
	ImGui::Begin(name.c_str());

	char title[25];
	sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
	ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
	sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
	ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));

	ImGui::End();
}

void PanelConfiguration::addFPS(float fps, float ms)
{
	if (fps_log.size() == 100)
	{
		for (int i = 0; i < fps_log.size() - 2; i++)
		{
			fps_log[i] = fps_log[i + 1];
			ms_log[i] = ms_log[i + 1];
		}

		fps_log[fps_log.size() - 1] = fps;
		ms_log[ms_log.size() - 1] = ms;
	}
	else
	{
		fps_log.push_back(fps);
		ms_log.push_back(ms);
	}
}
