#ifndef __MODULE_H__
#define __MODULE_H__

#include "Globals.h"
#include "JSONManager.h"

class Module
{
private :

	bool enabled;

public:

	Module(bool start_enabled = true)
	{}

	virtual ~Module()
	{}

	virtual bool Init(JSON_File* document) 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual bool SaveDefaultConfig(JSON_File* document)const
	{
		return true;
	}
};

#endif // !__MODULE_H__
