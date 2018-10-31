#ifndef __JASONMANAGER_H__
#define __JASONMANAGER_H__

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"// rapidjson's DOM-style API
#include "rapidjson/prettywriter.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include "Globals.h"
#include "MathGeoLib\MathGeoLib.h"

struct JSON_Value
{
public:

	JSON_Value(rapidjson::Document::AllocatorType* allocator): allocator(allocator)
	{ value = new rapidjson::Value(rapidjson::kObjectType); }
	
	~JSON_Value();

	//This will remove all added elements of the JSON_Value
	void convertToArray();

	void addInt(const char* name, int value);
	void addUint(const char* name, uint value);
	void addFloat(const char* name, float value);
	void addString(const char* name, const char* value);
	void addBool(const char* name, bool value);
	void addVector(const char* name, float* vec, int vector_size);
	void addVector3(const char* name, float3 vec);
	void addQuat(const char* name, Quat quat);

	int getInt(const char* name);
	uint getUint(const char* name);
	float getFloat(const char* name);
	const char* getString(const char* name);
	bool getBool(const char* name);
	float* getVector(const char* name, int vector_size);
	float3 getVector3(const char* name);
	Quat getQuat(const char* name);

	JSON_Value* createValue();
	void addValue(const char* name, JSON_Value* value);
	JSON_Value* getValue(const char* name);

	void setValue(rapidjson::Value* value);
	rapidjson::Value* getRapidJSONValue();

private:

	rapidjson::Value* value = nullptr;
	rapidjson::Document::AllocatorType* allocator = nullptr;
};

struct JSON_File
{
public:

	JSON_File(rapidjson::FileWriteStream* os, FILE* fp);
	JSON_File(rapidjson::FileReadStream* is, FILE* fp);
	~JSON_File();

	bool Write();

	JSON_Value* createValue();
	void addValue(const char* name, JSON_Value* value);
	JSON_Value* getValue(const char* name);

	void closeFile();

private:

	FILE* fp = nullptr;

	rapidjson::Document* document = nullptr;
	rapidjson::FileWriteStream* os = nullptr;
	rapidjson::FileReadStream* is = nullptr;

	rapidjson::Document::AllocatorType* allocator = nullptr;
};

class JSONManager
{
public:
	JSONManager();
	~JSONManager();

	JSON_File* openReadFile(const char* path);
	JSON_File* openWriteFile(const char* path);
	void closeFile(JSON_File* file);
};

#endif // !__JASONMANAGER_H__