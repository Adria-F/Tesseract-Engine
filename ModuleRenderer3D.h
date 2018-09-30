#pragma once
#include "Module.h"
#include "Light.h"

#include <list>
#include <vector>

using namespace std;

#define MAX_LIGHTS 8

class MSphere;

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init(rapidjson::Document& document);
	bool Start();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);

	bool Save(rapidjson::Document& document, rapidjson::FileWriteStream& os);
	bool Load(rapidjson::Document& document);

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	bool Depth_Test = true;
	bool Cull_Face = true;
	bool Lighting = true;
	bool Color_Material = true;
	bool Texture_2D = false;
	bool Wireframe = false;

	vector<float> box;
	vector<float> box2;
	vector<uint> indices;

	MSphere* sphere = nullptr;

	uint my_id = 0;
	uint my_ids = 0;
};