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
class GameObject;

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

	uint num_texCoords = 0;
	float* texCoords=nullptr;

	vector<float> faceNormals;

	AABB boundingBox;

	void Draw();
	void calculateNormals();
	void GenerateBuffer();


};

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init(JSON_File* document);
	bool Start();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void pushMesh(Mesh* mesh);

	void OnResize(int width, int height);


	bool Save(JSON_File* document)const;
	bool Load(JSON_File* document);

	void CalculateGlobalMatrix(GameObject* gameOject);

public:

	GLuint FramebufferName = 0;
	GLuint renderedTexture = 0;
	GLuint depthrenderbuffer = 0;

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;

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
	bool ShowQT = false;
	bool Frustum_Culling = true;
	bool ShowClickRay = false;

	bool changedFOV = false;

	uint buff_id = 0;

	//Clicking ray debug
	float3 clickA = float3::zero;
	float3 clickB = float3::zero;

	list<Mesh*> meshes;
};

#endif // !__MODULERENDERER3D_H__
