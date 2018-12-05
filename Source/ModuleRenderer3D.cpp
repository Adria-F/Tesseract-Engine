#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleCamera3D.h"
#include "ModuleGUI.h"
#include "Primitive.h"
#include "GameObject.h"

#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "ComponentTransformation.h"

#include "PanelScene.h"

#ifdef _DEBUG
//#define TEST_MEMORY_MANAGER
#include "mmgr/mmgr.h"
#endif

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Glew/libx86/glew32.lib") /* link Microsoft OpenGL lib   */

ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module( start_enabled)
{
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init(JSON_File* document)
{
	JSON_Value* rendererConfig = document->getValue("renderer");
	if (rendererConfig != nullptr)
	{
		vsync = rendererConfig->getBool("Vsync");
		Depth_Test = rendererConfig->getBool("Depth_Test");
		Cull_Face = rendererConfig->getBool("Cull_Face");
		Lighting = rendererConfig->getBool("Lighting");
		Color_Material = rendererConfig->getBool("Color_Material");
		Texture_2D = rendererConfig->getBool("Texture_2D");
		Wireframe = rendererConfig->getBool("Wireframe");
		Normals = rendererConfig->getBool("Normals");
		Faces = rendererConfig->getBool("Faces");
		ShowBB = rendererConfig->getBool("ShowBB");
		ShowQT = rendererConfig->getBool("ShowQT");
		Frustum_Culling = rendererConfig->getBool("Frustum_Culling");
		ShowClickRay = rendererConfig->getBool("ShowClickRay");
	}

	LOG("Creating 3D Renderer context");
	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	GLenum error = glewInit();
	if (error != GL_NO_ERROR)
	{
		LOG("Error initializing glew! Error: %s\n", gluErrorString(error));
		ret = false;
	}

	if(ret == true)
	{
		LOG("Using Glew %s", glewGetString(GLEW_VERSION));

		LOG("Vendor: %s", glGetString(GL_VENDOR));
		LOG("Renderer: %s", glGetString(GL_RENDERER));
		LOG("OpenGL version supported %s", glGetString(GL_VERSION));
		LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

		//Use Vsync
		if(vsync && SDL_GL_SetSwapInterval(1) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix. Specify which matrix is the current matrix
		glMatrixMode(GL_PROJECTION);//Applies subsequent matrix operations to the projection matrix stack. (screen position)
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix.Specify which matrix is the current matrix(camera position)
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		//specify implementation of The most correct, or highest quality, option should be chosen.
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);//Specifies the depth value used when the depth buffer is cleared.
		
		//Initialize clear color
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);//clear values for the color buffers. The initial values are all 0.


		//specify pixel arithmetic glBlendFunc(rgb source blending factors, rgb destination blending factors)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		if (Depth_Test)
			glEnable(GL_DEPTH_TEST);
		if (Cull_Face)
			glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		if (Lighting)
			glEnable(GL_LIGHTING);
		if (Color_Material)
			glEnable(GL_COLOR_MATERIAL);
		if (Texture_2D)
			glEnable(GL_TEXTURE_2D);
	}

	// Projection matrix for
	OnResize(App->window->width, App->window->height);

	//Texture Test------------------------

	GLubyte checkImage[CHECKERS_H][CHECKERS_W][4];
	for (int i = 0; i < CHECKERS_H; i++) 
	{
		for (int j = 0; j < CHECKERS_W; j++)
		{
			int c = ((((i&0x8) == 0) ^ (((j&0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)c;
		}
	}

	//Texture Buffer------------------------

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &buff_id);
	glBindTexture(GL_TEXTURE_2D, buff_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_W, CHECKERS_H,0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	glBindTexture(GL_TEXTURE_2D, 0);

	return ret;
}

bool ModuleRenderer3D::Start()
{
	bool ret = true;

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	glViewport(0, 0, App->window->width, App->window->height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if (changedFOV)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glLoadMatrixf(App->camera->camera->getProjectionMatrix());
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		changedFOV = false;
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->camera->getViewMatrix());

	// light 0 on cam pos
	//lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	CalculateGlobalMatrix(App->scene_intro->root);

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	if (Wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Draw Scene  ---------------------------	

	App->scene_intro->Draw();

	MPlane base_plane(0, 1, 0, 0);
	base_plane.axis = true;
	base_plane.Render();

	if (ShowClickRay) //Debug click ray
	{
		glLineWidth(2.0f);

		glBegin(GL_LINES);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glVertex3f(clickA.x, clickA.y, clickA.z);
		glVertex3f(clickB.x, clickB.y, clickB.z);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		glEnd();

		glLineWidth(1.0f);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, GameFramebufferName);
	glViewport(0, 0, App->window->width, App->window->height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	float* ViewM=App->camera->camera->getViewMatrix();
	float* ProjectionM= ViewM = App->camera->camera->getProjectionMatrix();

	if (App->scene_intro->activeCamera != nullptr)
	{
		ViewM = App->scene_intro->activeCamera->camera->getViewMatrix();
		ProjectionM = App->scene_intro->activeCamera->camera->getProjectionMatrix();
	}

	if (changedFOV)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glLoadMatrixf(ProjectionM);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		changedFOV = false;
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(ViewM);

	App->scene_intro->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	App->gui->Draw();

	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");
	
	SDL_GL_DeleteContext(context);

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	glDeleteFramebuffers(1, &FramebufferName);
	glDeleteTextures(1, &renderedTexture);
	glDeleteRenderbuffers(1,&depthrenderbuffer);

	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	glGenTextures(1, &renderedTexture);

	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, App->window->width, App->window->height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, App->window->width, App->window->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);

	glDeleteFramebuffers(1, &GameFramebufferName);
	glDeleteTextures(1, &GamerenderedTexture);
	glDeleteRenderbuffers(1, &Gamedepthrenderbuffer);

	glGenFramebuffers(1, &GameFramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, GameFramebufferName);

	glGenTextures(1, &GamerenderedTexture);

	glBindTexture(GL_TEXTURE_2D, GamerenderedTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, App->window->width, App->window->height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenRenderbuffers(1, &Gamedepthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, Gamedepthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, App->window->width, App->window->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, Gamedepthrenderbuffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GamerenderedTexture, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG("ERROR");
	}

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glLoadMatrixf(App->camera->camera->getProjectionMatrix());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}


bool ModuleRenderer3D::SaveDefaultConfig(JSON_File* document)const
{
	JSON_Value* renderer = document->createValue();
	
	renderer->addBool("Vsync", false);
	renderer->addBool("Depth_Test", true);
	renderer->addBool("Cull_Face", true);
	renderer->addBool("Lighting", false);
	renderer->addBool("Color_Material", true);
	renderer->addBool("Texture_2D", true);
	renderer->addBool("Wireframe", false);
	renderer->addBool("Normals", false);
	renderer->addBool("Faces", false);
	renderer->addBool("ShowBB", false);
	renderer->addBool("ShowQT", false);
	renderer->addBool("Frustum_Culling", true);
	renderer->addBool("ShowClickRay", false);

	document->addValue("renderer", renderer);

	return true;
}

void ModuleRenderer3D::CalculateGlobalMatrix(GameObject* gameObject)
{
	ComponentTransformation* transform = (ComponentTransformation*)gameObject->GetComponent(TRANSFORMATION);

	if (transform != nullptr)
	{
		if (gameObject->parent == nullptr)
		{
			transform->globalMatrix = transform->localMatrix;
		}
		else
		{
			transform->globalMatrix = ((ComponentTransformation*)gameObject->parent->GetComponent(TRANSFORMATION))->globalMatrix*transform->localMatrix;
		}


		for (std::list<GameObject*>::iterator it_c = gameObject->childs.begin(); it_c != gameObject->childs.end(); it_c++)
		{
			CalculateGlobalMatrix((*it_c));
		}
	}
}
