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

	if (fp != nullptr)
		return new JSON_File(new rapidjson::FileReadStream(fp, readBuffer, sizeof(readBuffer)), fp);
	else
		return nullptr;
}

JSON_File* JSONManager::openWriteFile(const char * path)
{
	FILE* fp = fopen(path, "wb");
	char writeBuffer[65536];

	if (fp != nullptr)
		return new JSON_File(new rapidjson::FileWriteStream(fp, writeBuffer, sizeof(writeBuffer)), fp);
	else
		return nullptr;
}

void JSONManager::closeFile(JSON_File* file)
{
	if (file != nullptr)
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
	int size = allocatedValues.size();
	for (int i = 0; i < size; i++)
	{
		RELEASE(allocatedValues[i]);
	}
	allocatedValues.clear();

	RELEASE(document);

	RELEASE(is);
	RELEASE(os);
}

bool JSON_File::Write()
{
	if (os != nullptr)
	{
		//rapidjson::Writer<rapidjson::FileWriteStream> writer(*os);
		rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(*os);
		document->Accept(writer);		

		return true;
	}
	else
		return false;
}

JSON_Value* JSON_File::createValue()
{
	JSON_Value* ret = new JSON_Value(allocator);
	allocatedValues.push_back(ret);
	return ret;
}

void JSON_File::addValue(const char * name, JSON_Value* value)
{
	std::string str = name;
	rapidjson::Value index(str.c_str(), str.size(), *allocator);
	document->AddMember(index, *value->getRapidJSONValue(), *allocator);
}

JSON_Value* JSON_File::getValue(const char* name)
{
	if (document->IsObject() && document->HasMember(name))
	{
		rapidjson::Value& value = document->operator[](name);
		JSON_Value* ret = new JSON_Value(allocator);
		allocatedValues.push_back(ret);
		ret->getRapidJSONValue()->CopyFrom(value, *allocator, false);

		return ret;
	}

	return nullptr;
}

JSON_Value::~JSON_Value()
{
	int size = allocatedValues.size();
	for (int i = 0; i < size; i++)
	{
		RELEASE(allocatedValues[i]);
	}
	allocatedValues.clear();

	RELEASE(value);
}

void JSON_Value::convertToArray()
{
	RELEASE(value);
	value = new rapidjson::Value(rapidjson::kArrayType);
}

void JSON_Value::addInt(const char* name, int value)
{
	std::string str = name;
	rapidjson::Value index(str.c_str(), str.size(), *allocator);
	this->value->AddMember(index, value, *allocator);
}

void JSON_Value::addUint(const char * name, uint value)
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

void JSON_Value::addVector2(const char* name, float2 vec)
{
	std::string str = name;
	rapidjson::Value index(str.c_str(), str.size(), *allocator);

	rapidjson::Value a(rapidjson::kArrayType);
	a.PushBack(vec.x, *allocator);
	a.PushBack(vec.y, *allocator);

	this->value->AddMember(index, a, *allocator);
}

void JSON_Value::addVector3(const char * name, float3 vec)
{
	std::string str = name;
	rapidjson::Value index(str.c_str(), str.size(), *allocator);

	rapidjson::Value a(rapidjson::kArrayType);
	a.PushBack(vec.x, *allocator);
	a.PushBack(vec.y, *allocator);
	a.PushBack(vec.z, *allocator);

	this->value->AddMember(index, a, *allocator);
}

void JSON_Value::addQuat(const char * name, Quat quat)
{
	std::string str = name;
	rapidjson::Value index(str.c_str(), str.size(), *allocator);

	rapidjson::Value a(rapidjson::kArrayType);
	a.PushBack(quat.x, *allocator);
	a.PushBack(quat.y, *allocator);
	a.PushBack(quat.z, *allocator);
	a.PushBack(quat.w, *allocator);

	this->value->AddMember(index, a, *allocator);
}

void JSON_Value::addTransform(const char * name, float4x4 mat)
{
	std::string str = name;
	rapidjson::Value index(str.c_str(), str.size(), *allocator);

	rapidjson::Value a(rapidjson::kArrayType);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			a.PushBack(mat.v[i][j], *allocator);
		}
	}

	this->value->AddMember(index, a, *allocator);
}

void JSON_Value::setUint(const char* name, uint value)
{
	rapidjson::Value newValue(value);

	if (this->value->HasMember(name)) //If it exists modify it
		this->value->operator[](name) = newValue;
	else
		this->addUint(name, value); //if not, set a new one
}

int JSON_Value::getInt(const char* name)
{	
	if (value->HasMember(name))
		return value->operator[](name).GetInt();
	else
		return 0;
}

uint JSON_Value::getUint(const char * name)
{
	if (value->HasMember(name))
		return value->operator[](name).GetUint();
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

float2 JSON_Value::getVector2(const char * name)
{
	if (value->HasMember(name))
	{
		rapidjson::Value& a = value->operator[](name);
		if (a.IsArray() && a.Size() >= 2)
		{
			float2 ret;
			ret.x = a[0].GetFloat();
			ret.y = a[1].GetFloat();

			return ret;
		}
	}

	return float2();
}

float3 JSON_Value::getVector3(const char * name)
{
	if (value->HasMember(name))
	{
		rapidjson::Value& a = value->operator[](name);
		if (a.IsArray() && a.Size() >= 3)
		{
			float3 ret;
			ret.x = a[0].GetFloat();
			ret.y = a[1].GetFloat();
			ret.z = a[2].GetFloat();

			return ret;
		}
	}

	return float3();
}

Quat JSON_Value::getQuat(const char * name)
{
	if (value->HasMember(name))
	{
		rapidjson::Value& a = value->operator[](name);
		if (a.IsArray() && a.Size() >= 4)
		{
			Quat ret;
			ret.x = a[0].GetFloat();
			ret.y = a[1].GetFloat();
			ret.z = a[2].GetFloat();
			ret.w = a[3].GetFloat();

			return ret;
		}
	}

	return Quat();
}

float4x4 JSON_Value::getTransform(const char * name)
{
	if (value->HasMember(name))
	{
		rapidjson::Value& a = value->operator[](name);
		if (a.IsArray() && a.Size() >= 16)
		{
			float4x4 ret;
			int count = 0;
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					ret.v[i][j] = a[count].GetFloat();
					count++;
				}
			}

			return ret;
		}
	}	

	return float4x4();
}

JSON_Value * JSON_Value::createValue()
{
	JSON_Value* ret = new JSON_Value(allocator);
	allocatedValues.push_back(ret);
	return ret;
}

void JSON_Value::addValue(const char * name, JSON_Value * value)
{
	if (this->value->GetType() == rapidjson::kObjectType)
	{
		std::string str = name;
		rapidjson::Value index(str.c_str(), str.size(), *allocator);
		this->value->AddMember(index, *value->getRapidJSONValue(), *allocator);
	}
	else if (this->value->GetType() == rapidjson::kArrayType)
	{
		this->value->PushBack(*value->getRapidJSONValue(), *allocator);
	}
}

JSON_Value* JSON_Value::getValue(const char* name)
{
	if (value->IsObject() && value->HasMember(name))
	{
		rapidjson::Value& trueValue = value->operator[](name);
		JSON_Value* ret = new JSON_Value(allocator);
		allocatedValues.push_back(ret);
		ret->getRapidJSONValue()->CopyFrom(trueValue, *allocator, false);

		return ret;
	}

	return nullptr;
}

JSON_Value* JSON_Value::getValueFromArray(int index)
{
	if (value->IsArray() && value->Size() > index)
	{
		rapidjson::Value& trueValue = value->operator[](index);
		JSON_Value* ret = new JSON_Value(allocator);
		allocatedValues.push_back(ret);
		ret->getRapidJSONValue()->CopyFrom(trueValue, *allocator, false);

		return ret;
	}

	return nullptr;
}

void JSON_Value::setValue(const char* name, JSON_Value* value)
{
	if (this->value->HasMember(name))
		this->value->operator[](name) = *value->getRapidJSONValue(); //If it exists modify it
	else
		this->addValue(name, value); //if not, set a new one
}

void JSON_Value::setValue(rapidjson::Value * value)
{
	*this->value = *value;
}

rapidjson::Value* JSON_Value::getRapidJSONValue()
{
	return value;
}

void JSON_File::closeFile()
{
	fclose(fp);
}