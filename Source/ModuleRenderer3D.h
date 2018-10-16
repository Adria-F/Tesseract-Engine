#ifndef __MODULERENDERER3D_H__
#define	__MODULERENDERER3D_H__

#include "Module.h"
#include "Light.h"

#include <list>
#include <vector>

using namespace std;

#define MAX_LIGHTS 8
#define CHECKERS_H 256
#define CHECKERS_W 256


class MSphere;
class Component;
class ComponentMesh;
class ComponentTexture;

struct Mesh
{
	~Mesh();

	std::string name;

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
	uint texWidth = 0;
	uint texHeight = 0;

	vector<float> faceNormals;

	AABB boundingBox;

	vec3 position = { 0.0f,0.0f,0.0f };
	vec3 scale = { 1.0,1.0f,1.0f };
	Quat rotation = { 0.0f,0.0f,0.0f,0.0f };

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

	void DrawBB(const AABB& BB,vec3 color)const;

	bool Save(rapidjson::Document& document, rapidjson::FileWriteStream& os)const;
	bool Load(rapidjson::Document& document);

	ComponentMesh* CreateComponentMesh();
	ComponentTexture* CreateComponentTexture();



public:

	GLuint FramebufferName = 0;
	GLuint renderedTexture = 0;
	GLuint depthrenderbuffer = 0;

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	bool vsync = false;
	bool Depth_Test = false;
	bool Cull_Face = false;
	bool Lighting = false;
	bool Color_Material = false;
	bool Texture_2D = false;
	bool Wireframe = false;
	bool Normals = false;
	bool Faces = false;
	bool ShowBB = false;

	uint buff_id = 0;

	list<Mesh*> meshes;
};

#endif // !__MODULERENDERER3D_H__
