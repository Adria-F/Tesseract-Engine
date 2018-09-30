#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleSceneIntro.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics3D.h"
#include "ModuleGUI.h"
#include "Primitive.h"

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
bool ModuleRenderer3D::Init(rapidjson::Document& document)
{
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
		if(VSYNC && SDL_GL_SetSwapInterval(1) < 0)
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
		glClearColor(0.f, 0.f, 0.f, 1.f);//clear values for the color buffers. The initial values are all 0.


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
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	return ret;
}

bool ModuleRenderer3D::Start()
{
	bool ret = true;

	box = { 25.0f, 10.0f, 0.0f,35.0f, 0.0f, 0.0f,25.0f, 0.0f, 0.0f,
		25.0f, 10.0f, 0.0f,35.0f, 10.0f, 0.0f,35.0f, 0.0f, 0.0f,

	35.0f, 10.0f, 0.0f,35.0f, 0.0f, 10.0f,35.0f, 0.0f, 0.0f,
	35.0f, 10.0f, 0.0f,35.0f, 10.0f, 10.0f,35.0f, 0.0f, 10.0f,

	25.0f, 10.0f, 10.0f,35.0f, 0.0f, 10.0f,35.0f, 10.0f, 10.0f,
	25.0f, 10.0f, 10.0f,25.0f, 0.0f, 10.0f,35.0f, 0.0f, 10.0f,

	25.0f, 0.0f, 0.0f,25.0f, 0.0f, 10.0f,25.0f, 10.0f, 10.0f,
	25.0f, 10.0f, 0.0f,25.0f, 0.0f, 0.0f,25.0f, 10.0f, 10.0f,

	35.0f, 10.0f, 0.0f,25.0f, 10.0f, 0.0f,25.0f, 10.0f, 10.0f,
	25.0f, 10.0f, 10.0f,35.0f, 10.0f, 10.0f,35.0f, 10.0f, 0.0f,

	35.0f, 0.0f, 10.0f,25.0f, 0.0f, 10.0f,35.0f, 0.0f, 0.0f,
	25.0f, 0.0f, 10.0f,25.0f, 0.0f, 0.0f ,35.0f, 0.0f, 0.0f };


	glGenBuffers(1,(GLuint*)&(my_id));
	glBindBuffer(GL_ARRAY_BUFFER, my_id);
	glBufferData(GL_ARRAY_BUFFER,sizeof(float)*3*36,&box[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	box2 = {-25.f,0.f,10.f,-15.f,0.f,10.f,-25.f,10.f,10.f,-15.f,10.f,10.f, // a(0),b(1),c(2),d(3)
		-25.f,0.f,0.f,-15.f,0.f,0.f,-25.f,10.f,0.f,-15.f,10.f,0.f };//e(4), f(5),g(6),h(7)

	indices = { 0,1,2, 1,3,2,
	3,1,5,  5,7,3,
	7,5,4,  6,7,4,
	6,4,0,  0,2,6,
	6,2,3,  6,3,7,
	0,4,5,  0,5,1};

	glGenBuffers(1, (GLuint*)&(my_ids));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_ids);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)* 36, &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Sphere --------------------------------
	sphere = new MSphere(10, 6, 12);

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	if (Wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Direct Mode --------------------------------
	glLineWidth(2.0f);
	glBegin(GL_TRIANGLES);

	glVertex3f(0.0f, 10.0f, 0.0f); glVertex3f(10.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 10.0f, 0.0f); glVertex3f(10.0f, 10.0f, 0.0f); glVertex3f(10.0f, 0.0f, 0.0f);

	glVertex3f(10.0f, 10.0f, 0.0f); glVertex3f(10.0f, 0.0f, 10.0f); glVertex3f(10.0f, 0.0f, 0.0f);
	glVertex3f(10.0f, 10.0f, 0.0f); glVertex3f(10.0f, 10.0f, 10.0f); glVertex3f(10.0f, 0.0f, 10.0f);

	glVertex3f(0.0f, 10.0f, 10.0f); glVertex3f(10.0f, 0.0f, 10.0f); glVertex3f(10.0f, 10.0f, 10.0f);
	glVertex3f(0.0f, 10.0f, 10.0f); glVertex3f(0.0f, 0.0f, 10.0f); glVertex3f(10.0f, 0.0f, 10.0f);

	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 10.0f); glVertex3f(0.0f, 10.0f, 10.0f);
	glVertex3f(0.0f, 10.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 10.0f, 10.0f);

	glVertex3f(10.0f, 10.0f, 0.0f); glVertex3f(0.0f, 10.0f, 0.0f); glVertex3f(0.0f, 10.0f, 10.0f);
	glVertex3f(0.0f, 10.0f, 10.0f); glVertex3f(10.0f, 10.0f, 10.0f); glVertex3f(10.0f, 10.0f, 0.0f);

	glVertex3f(10.0f, 0.0f, 10.0f); glVertex3f(0.0f, 0.0f, 10.0f); glVertex3f(10.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 10.0f); glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(10.0f, 0.0f, 0.0f);

	glEnd();
	glLineWidth(1.0f);

	//Vertex Array -----------------------------
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glBindBuffer(GL_ARRAY_BUFFER, my_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Indices ----------------------------------
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_ids);
	glVertexPointer(3, GL_FLOAT, 0, &box2[0]);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	
	glDisableClientState(GL_VERTEX_ARRAY);

	//Sphere Test ---------------------------
	sphere->Render();

	MPlane base_plane(0, 1, 0, 0);
	base_plane.axis = true;
	base_plane.Render();

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
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

bool ModuleRenderer3D::Save(rapidjson::Document& document, rapidjson::FileWriteStream& os) {
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

	document.AddMember("name", "render", allocator);

	rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
	return true;
}

bool ModuleRenderer3D::Load(rapidjson::Document& document) {
	return true;
}