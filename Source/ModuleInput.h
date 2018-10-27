#ifndef __MODULEINPUT_H__
#define __MODULEINPUT_H__

#include "Module.h"
#include "SDL/include/SDL.h"
#include "SDL\include\SDL_scancode.h"
#include "SDL\include\SDL_gamecontroller.h"

#define MAX_MOUSE_BUTTONS 5
#define MAX_BUTTONS 20

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{
public:

	ModuleInput(bool start_enabled = true);
	~ModuleInput();

	bool Init(JSON_File* document);
	update_status PreUpdate(float dt);
	bool CleanUp();

	KEY_STATE GetKey(int id) const
	{
		return keyboard[id];
	}

	KEY_STATE GetMouseButton(int id) const;

	KEY_STATE GetController(int id) const
	{
		return player_controll[id];
	}

	int GetMouseX() const
	{
		return mouse_x;
	}

	int GetMouseY() const
	{
		return mouse_y;
	}

	int GetMouseZ() const
	{
		return mouse_z;
	}

	int GetMouseXMotion() const
	{
		return mouse_x_motion;
	}

	int GetMouseYMotion() const
	{
		return mouse_y_motion;
	}

	bool Save(rapidjson::Document& document, rapidjson::FileWriteStream& os)const;
	bool Load(rapidjson::Document& document);

public:
	KEY_STATE* keyboard=nullptr;
	KEY_STATE mouse_buttons[MAX_MOUSE_BUTTONS];
	KEY_STATE player_controll[MAX_BUTTONS];

	SDL_GameController *controller = nullptr;

	int mouse_x;
	int mouse_y;
	int mouse_z;
	int mouse_x_motion;
	int mouse_y_motion;

	bool move_up = false;
	bool move_left = false;
	bool move_right = false;
	bool stop = false;

	//int mouse_z_motion;
};

#endif // !__MODULEINPUT_H__
