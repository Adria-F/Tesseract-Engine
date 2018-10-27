#include "JSONManager.h"
#include "Globals.h"

JSONManager::JSONManager()
{
}

JSONManager::~JSONManager()
{
}

JSON_File* JSONManager::openReadFile(const char * path)
{
	FILE* fp = fopen(path, "rb");
	char readBuffer[65536];

	return new JSON_File(new rapidjson::FileReadStream(fp, readBuffer, sizeof(readBuffer)), fp);
}

JSON_File* JSONManager::openWriteFile(const char * path)
{
	FILE* fp = fopen(path, "wb");
	char writeBuffer[65536];

	return new JSON_File(new rapidjson::FileWriteStream(fp, writeBuffer, sizeof(writeBuffer)), fp);
}

void JSONManager::closeFile(JSON_File* file)
{
	file->closeFile();

	RELEASE(file);
	file = nullptr;
}

// -------------------------------------------------------------------------------------

JSON_File::JSON_File(rapidjson::FileWriteStream* os, FILE* fp): os(os), fp(fp)
{
	document = new rapidjson::Document();
	document->SetObject();

	allocator = &document->GetAllocator();
}

JSON_File::JSON_File(rapidjson::FileReadStream* is, FILE* fp): is(is), fp(fp)
{
	document = new rapidjson::Document();
	document->ParseStream(*is);

	allocator = &document->GetAllocator();
}

JSON_File::~JSON_File()
{
	RELEASE(document);

	RELEASE(is);
	RELEASE(os);
}

bool JSON_File::Write()
{
	if (os != nullptr)
	{
		rapidjson::Writer<rapidjson::FileWriteStream> writer(*os);
		document->Accept(writer);

		return true;
	}
	else
		return false;
}

JSON_Value* JSON_File::createValue()
{
	return new JSON_Value(allocator);
}

void JSON_File::addValue(const char * name, JSON_Value* value)
{
	std::string str = name;
	rapidjson::Value index(str.c_str(), str.size(), *allocator);
	document->AddMember(index, *value->getRapidJSONValue(), *allocator);
}

JSON_Value* JSON_File::getValue(const char* name)
{
	if (document->HasMember(name))
	{
		rapidjson::Value& value = document->operator[](name);
		JSON_Value* ret = new JSON_Value(allocator);
		ret->getRapidJSONValue()->CopyFrom(value, *allocator, false);

		return ret;
	}

	return nullptr;
}

JSON_Value::~JSON_Value()
{
	RELEASE(value);
}

void JSON_Value::addInt(const char* name, int value)
{
	std::string str = name;
	rapidjson::Value index(str.c_str(), str.size(), *allocator);
	this->value->AddMember(index, value, *allocator);
}

void JSON_Value::addFloat(const char * name, float value)
{
	std::string str = name;
	rapidjson::Value index(str.c_str(), str.size(), *allocator);
	this->value->AddMember(index, value, *allocator);
}

void JSON_Value::addString(const char * name, const char * value)
{
	std::string str = name;
	rapidjson::Value index(str.c_str(), str.size(), *allocator);
	std::string str2 = value;
	rapidjson::Value val(str2.c_str(), str2.size(), *allocator);
	this->value->AddMember(index, val, *allocator);
}

void JSON_Value::addBool(const char * name, bool value)
{
	std::string str = name;
	rapidjson::Value index(str.c_str(), str.size(), *allocator);
	this->value->AddMember(index, value, *allocator);
}

void JSON_Value::addVector(const char * name, float * vec, int vector_size)
{
	std::string str = name;
	rapidjson::Value index(str.c_str(), str.size(), *allocator);

	rapidjson::Value a(rapidjson::kArrayType);
	for (int i = 0; i < vector_size; i++)
	{
		a.PushBack(vec[i], *allocator);
	}

	this->value->AddMember(index, a, *allocator);
}

int JSON_Value::getInt(const char* name)
{	
	if (value->HasMember(name))
		return value->operator[](name).GetInt();
	else
		return 0;
}

float JSON_Value::getFloat(const char * name)
{
	if (value->HasMember(name))
		return value->operator[](name).GetFloat();
	else
		return 0.0f;
}

const char * JSON_Value::getString(const char * name)
{
	if (value->HasMember(name))
		return value->operator[](name).GetString();
	else
		return "";
}

bool JSON_Value::getBool(const char * name)
{
	if (value->HasMember(name))
		return value->operator[](name).GetBool();
	else
		return false;
}

float* JSON_Value::getVector(const char * name, int vector_size)
{
	if (value->HasMember(name))
	{
		rapidjson::Value& a = value->operator[](name);
		if (a.IsArray() && a.Size() >= vector_size)
		{
			float* ret = new float[vector_size]; //MEMLEAK
			for (int i = 0; i < vector_size; i++)
			{
				ret[i] = a[i].GetFloat();
			}

			return ret;
		}
	}

	return nullptr;
}

JSON_Value * JSON_Value::getValue(const char * name)
{
	if (value->HasMember(name))
	{
		rapidjson::Value& trueValue = value->operator[](name);
		JSON_Value* ret = new JSON_Value(allocator);
		ret->getRapidJSONValue()->CopyFrom(trueValue, *allocator, false);

		return ret;
	}

	return nullptr;
}

void JSON_Value::setValue(rapidjson::Value * value)
{
	if (value != nullptr)
		RELEASE(value);

	this->value = value;
}

rapidjson::Value* JSON_Value::getRapidJSONValue()
{
	return value;
}

void JSON_File::closeFile()
{
	fclose(fp);
}
