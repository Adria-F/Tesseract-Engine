#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Globals.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"
#include "ModuleGUI.h"
#include "PanelConfiguration.h"
#include "ModuleInput.h"
#include "MathGeoLib/MathGeoLib.h"

#include "mmgr/mmgr.h"

PanelConfiguration::PanelConfiguration(const char * name) : Panel(name),
fps_log(FPS_LOG_SIZE), ms_log(FPS_LOG_SIZE), memory_log(MEMORY_LOG_SIZE)
{
	nWidth = SCREEN_WIDTH * SCREEN_SIZE;
	nHeight = SCREEN_HEIGHT * SCREEN_SIZE;
	
	resizable = WIN_RESIZABLE;

	brightness = SDL_GetWindowBrightness(App->window->window);

	SDL_GetDisplayMode(NULL, NULL, &mode);

	active = true;
}

PanelConfiguration::~PanelConfiguration()
{
}

void PanelConfiguration::Draw()
{
	ImGui::SetNextWindowSizeConstraints({ 100, 400 }, { 300, (float)App->window->height });
	ImGui::Begin(name.c_str(),&active, ImGuiWindowFlags_NoFocusOnAppearing);

	if (ImGui::CollapsingHeader("Application"))
	{
		ImGui::PushItemWidth(200.0f);
		static char app_name[120];
		strcpy_s(app_name, 120, App->getAppName());
		if (ImGui::InputText("App Name", app_name, 120, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			App->setAppName(app_name);

		static int framerateCap = App->getFramerateCap();
		if (ImGui::SliderInt("MaxFPS", &framerateCap, 1, 120))
			App->setFramerateCap(framerateCap);
		ImGui::PopItemWidth();

		char title[25];
		sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
		ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
		
		sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
		ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));

		sMStats stats = m_getMemoryStatistics();
		addMemory((float)stats.totalReportedMemory);

		/*LOG("----------------------------------");
		LOG("Total Reported Mem: %u", stats.totalReportedMemory);
		LOG("Total Actual Mem: %u", stats.totalActualMemory);
		LOG("Peak Reported Mem: %u", stats.peakReportedMemory);
		LOG("Peak Actual Mem: %u", stats.peakActualMemory);
		LOG("Accumulated Reported Mem: %u", stats.accumulatedReportedMemory);
		LOG("Accumulated Actual Mem: %u", stats.accumulatedActualMemory);
		LOG("Accumulated Alloc Unit Count: %u", stats.accumulatedAllocUnitCount);
		LOG("Total Alloc Unit Count: %u", stats.totalAllocUnitCount);
		LOG("Peak Alloc Unit Count: %u", stats.peakAllocUnitCount);*/

		ImGui::PlotHistogram("##memory", &memory_log[0], memory_log.size(), 0, "Memory Consumption", 0.0f, (float)stats.peakReportedMemory * 1.2f, ImVec2(310, 100));
	}

	if (ImGui::CollapsingHeader("Window"))
	{
		ImGui::PushItemWidth(200.0f);
		if (ImGui::SliderInt("Width", &nWidth, 0, 1920) || ImGui::SliderInt("Height", &nHeight, 0, 1080))
		{
			SDL_SetWindowSize(App->window->window, nWidth, nHeight);
	/*		App->window->OnResize(nWidth, nHeight);
			App->renderer3D->OnResize(nWidth, nHeight);*/
		}

		if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f))
		{
			SDL_SetWindowBrightness(App->window->window, brightness);
		}
		ImGui::PopItemWidth();

		ImGui::Text("Refresh rate: %d", mode.refresh_rate);

		if (ImGui::Checkbox("FullScreen", &fullscreen))
		{
			if (fullscreen)
			{
				SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN);
				SDL_GetWindowSize(App->window->window, &nWidth, &nHeight);
				/*App->window->OnResize(nWidth, nHeight);
				App->renderer3D->OnResize(nWidth, nHeight);
				App->gui->OnResize(nWidth, nHeight);*/
			}
			else
				SDL_SetWindowFullscreen(App->window->window, 0);
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Resizable", &resizable))
		{
			
			if (resizable)
				SDL_SetWindowResizable(App->window->window, (SDL_bool)1);
			else
				SDL_SetWindowResizable(App->window->window, (SDL_bool)0);
		}

		ImGui::NewLine();
		if (ImGui::Checkbox("Borderless", &borderless))
		{
			if (borderless)
				SDL_SetWindowBordered(App->window->window, (SDL_bool)(!borderless));
			else
				SDL_SetWindowBordered(App->window->window, (SDL_bool)1);
		}
		
		ImGui::SameLine();
		if (ImGui::Checkbox("Full Desktop", &full_desktop))
		{
			if (full_desktop) {
				SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			}
			else
				SDL_SetWindowFullscreen(App->window->window, 0);
		}
	}

	if (ImGui::CollapsingHeader("Renderer 3D"))
	{
		if (ImGui::Checkbox("Depth Test", &App->renderer3D->Depth_Test))
		{
			if (App->renderer3D->Depth_Test)
				glEnable(GL_DEPTH_TEST);

			else
				glDisable(GL_DEPTH_TEST);
		}
		if (ImGui::Checkbox("Cull Face", &App->renderer3D->Cull_Face))
		{
			if (App->renderer3D->Cull_Face)
				glEnable(GL_CULL_FACE);

			else
				glDisable(GL_CULL_FACE);
		}
		if (ImGui::Checkbox("Lighting", &App->renderer3D->Lighting))
		{
			if (App->renderer3D->Lighting)
				glEnable(GL_LIGHTING);

			else
				glDisable(GL_LIGHTING);
		}
		if (ImGui::Checkbox("Color Material", &App->renderer3D->Color_Material))
		{
			if (App->renderer3D->Color_Material)
				glEnable(GL_COLOR_MATERIAL);

			else
				glDisable(GL_COLOR_MATERIAL);
		}
		if (ImGui::Checkbox("Texture 2D", &App->renderer3D->Texture_2D))
		{
			if (App->renderer3D->Texture_2D)
				glEnable(GL_TEXTURE_2D);

			else
				glDisable(GL_TEXTURE_2D);
		}
		ImGui::Checkbox("Wireframe", &App->renderer3D->Wireframe);
		ImGui::Checkbox("Normals", &App->renderer3D->Normals);
		ImGui::Checkbox("Face Normals", &App->renderer3D->Faces);
		ImGui::Checkbox("Bounding Box", &App->renderer3D->ShowBB);
		
	}

	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::Text("Camera Position:");
		ImGui::Text("X: "); ImGui::SameLine();
		ImGui::Text(std::to_string(App->camera->Position.x).c_str());
		ImGui::Text("Y: "); ImGui::SameLine();
		ImGui::Text(std::to_string(App->camera->Position.y).c_str());
		ImGui::Text("Z: "); ImGui::SameLine();
		ImGui::Text(std::to_string(App->camera->Position.z).c_str());

		ImGui::PushItemWidth(100.0f);
		ImGui::InputFloat("Camera Speed", &App->camera->cameraSpeed);
		ImGui::InputFloat("Mouse Sensitivity", &App->camera->mouseSensitivity);
		ImGui::InputFloat("Wheel Sensitivity", &App->camera->wheelSensitivity);
		ImGui::PopItemWidth();
	}

	if (ImGui::CollapsingHeader("Input"))
	{
		ImGui::Text("Mouse Position:");
		ImGui::Text("X: %d | Y: %d", App->input->mouse_x, App->input->mouse_y);
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

void PanelConfiguration::addMemory(float memory)
{
	//memory /= 1000000;
	for (uint i = 0; i < MEMORY_LOG_SIZE - 1; ++i)
	{
		memory_log[i] = memory_log[i + 1];
	}

	memory_log[MEMORY_LOG_SIZE - 1] = memory;
}
