#ifndef __MODULERENDERER3D_H__
#define	__MODULERENDERER3D_H__

#include "Module.h"
#include "Light.h"

#include <list>
#include <vector>
#include <queue>

using namespace std;

#define MAX_LIGHTS 8
#define CHECKERS_H 256
#define CHECKERS_W 256


class MSphere;
class Component;
class ComponentMesh;
class ComponentMaterial;
class ComponentCamera;
class GameObject;

struct closerToCamera
{
	bool operator()(const GameObject* Obj_1, const GameObject* Obj_2)const;
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

	void OnResize(int width, int height);

	bool SaveDefaultConfig(JSON_File* document)const;

	void CalculateGlobalMatrix(GameObject* gameObject);

	void drawAllGameObjects();
	void drawGameObject(const GameObject* gameObject);
	void DrawBB(const AABB& BB, vec color) const;

	void addToRenderBuffer(const GameObject* gameObject);
	void removeFromRenderBuffer(GameObject* gameObject);

public:

	GLuint FramebufferName = 0;
	GLuint renderedTexture = 0;
	GLuint depthrenderbuffer = 0;

	GLuint GameFramebufferName = 0;
	GLuint GamerenderedTexture = 0;
	GLuint Gamedepthrenderbuffer = 0;

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

	bool changedSceneFOV = false;
	bool changedGameFOV = false;

	uint buff_id = 0;

	//Clicking ray debug
	float3 clickA = float3::zero;
	float3 clickB = float3::zero;

	vector<ComponentCamera*> cameras;

	/*list<Mesh*> meshes;*/
	std::priority_queue<const GameObject*, std::vector<const GameObject*>, closerToCamera> blendColorsBuffer;
	std::list<const GameObject*> renderBuffer;
};

#endif // !__MODULERENDERER3D_H__
