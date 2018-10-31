#include "Application.h"
#include "ModuleWindow.h"

using namespace rapidjson;

ModuleWindow::ModuleWindow(bool start_enabled) : Module(start_enabled)
{
	window = NULL;
	screen_surface = NULL;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init(JSON_File* document)
{
	LOG("Init SDL window & surface");
	bool ret = true;

	JSON_Value* configWindow = document->getValue("window");
	if (configWindow != nullptr)
	{
		width = configWindow->getInt("width");
		height = configWindow->getInt("height");
		screen_size = configWindow->getInt("screen_size");

		fullscreen = configWindow->getBool("fullscreen");
		resizable = configWindow->getBool("resizable");
		borderless = configWindow->getBool("borderless");
		fullscreen_desktop = configWindow->getBool("fullscreen_desktop");
		screen_margin_w = configWindow->getVector("screen_margin", 2)[0];
		screen_margin_h = configWindow->getVector("screen_margin", 2)[1];
	}

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		SDL_DisplayMode desktopSize;
		SDL_GetDesktopDisplayMode(0, &desktopSize);
		width = desktopSize.w - screen_margin_w;
		height = desktopSize.h - screen_margin_h;

		//Create window
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 3.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if(fullscreen == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(resizable == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(borderless == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(fullscreen_desktop == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(App->getAppName(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width * screen_size, height * screen_size, flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	OnResize(width*screen_size, height*screen_size);
	
	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	if (screen_surface != nullptr)
	{
		SDL_FreeSurface(screen_surface);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

bool ModuleWindow::Save(JSON_File* document) const
{
	JSON_Value* window = document->createValue();
	window->addInt("window Info", 2);
	window->convertToArray();
	JSON_Value* transform = window->createValue();
	JSON_Value* rotation = window->createValue();
	JSON_Value* scale = window->createValue();
	transform->addInt("TransX", 1);
	transform->addInt("TransY", 2);
	transform->addInt("TransZ", 3);
	rotation->addInt("RotX", 4);
	rotation->addInt("RotY", 5);
	rotation->addInt("RotZ", 6);
	scale->addInt("ScaleX", 7);
	scale->addInt("ScaleY", 8);
	scale->addInt("ScaleZ", 9);

	window->addValue("",transform);
	window->addValue("",rotation);
	window->addValue("",scale);
	document->addValue("window", window);

	return true;
}
bool ModuleWindow::Load(JSON_File* document) {
	return true;
}

void ModuleWindow::OnResize(int width, int height)
{
	this->width = width;
	this->height = height;
}
