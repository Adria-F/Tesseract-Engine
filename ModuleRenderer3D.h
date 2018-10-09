#pragma once
#include "Module.h"
#include "Light.h"

#include <list>
#include <vector>

using namespace std;

#define MAX_LIGHTS 8
#define CHECKERS_H 256
#define CHECKERS_W 256


class MSphere;

struct Mesh
{
	~Mesh();

	std::string name;

	uint id_vertices = 0; //to delete
	uint num_vertices = 0;
	float* vertices = nullptr;

	uint id_indices = 0;
	uint num_indices = 0;
	uint* indices = nullptr;

	uint num_normals = 0;
	float* normals = nullptr;

	vec3 color = { 0,0,0 };

	uint texture = 0;
	float* texCoords=nullptr;
	uint width = 0;
	uint height = 0;

	vector<float> faceNormals;

	AABB boundingBox;

	void Draw();
	void calculateNormals();
};

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

	void pushMesh(Mesh* mesh);

	void OnResize(int width, int height);

	void ChangeMeshTexture(const char* path);

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
	bool Normals = false;
	bool Faces = false;
	bool ShowBB = false;

	uint buff_id = 0;

	list<Mesh*> meshes;
};