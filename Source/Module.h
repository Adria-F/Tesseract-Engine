#ifndef __MODULE_H__
#define __MODULE_H__

#include "Globals.h"
#include "rapidjson/document.h"// rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filewritestream.h"

class Module
{
private :

	bool enabled;

public:

	Module(bool start_enabled = true)
	{}

	virtual ~Module()
	{}

	virtual bool Init(rapidjson::Document& document) 
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

	virtual bool Save(rapidjson::Document& document, rapidjson::FileWriteStream& os)const
	{
		rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
		rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
		return true;
	}

	virtual bool Load(rapidjson::Document& document)
	{
		return true;
	}

	/*virtual void OnCollision(PhysBody3D* body1, PhysBody3D* body2)
	{}*/
};

#endif // !__MODULE_H__
