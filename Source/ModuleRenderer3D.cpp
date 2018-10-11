#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleMeshLoader.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleCamera3D.h"
#include "ModuleGUI.h"
#include "Primitive.h"

#include "PanelScene.h"

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
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//clear values for the color buffers. The initial values are all 0.


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
	
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	glGenTextures(1, &renderedTexture);

	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, App->window->width, App->window->height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, App->window->width, App->window->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG("ERROR");
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	glViewport(0, 0, App->window->width, App->window->height);

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

	//Draw Scene  ---------------------------
	App->scene_intro->Draw();
	

	//Draw meshes
	for (list<Mesh*>::iterator it_m = meshes.begin(); it_m != meshes.end(); it_m++)
	{
		(*it_m)->Draw();
	}

	MPlane base_plane(0, 1, 0, 0);
	base_plane.axis = true;
	base_plane.Render();

	
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

void ModuleRenderer3D::pushMesh(Mesh* mesh)
{
	glGenBuffers(1, (GLuint*)&(mesh->id_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->num_indices, &mesh->indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	meshes.push_back(mesh);
}

void ModuleRenderer3D::OnResize(int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	glDeleteTextures(1, &renderedTexture);
	glGenTextures(1, &renderedTexture);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, App->window->width, App->window->height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ModuleRenderer3D::ChangeMeshTexture(const char * path)
{
	uint width, height;
	GLuint tex_id = App->mesh_loader->loadTexture(path, width, height);

	for (list<Mesh*>::iterator it_m = meshes.begin(); it_m != meshes.end(); it_m++)
	{
		//glDeleteTextures(1, (GLuint*)(*it_m)->texture);
		(*it_m)->texture = tex_id;
		(*it_m)->width = width;
		(*it_m)->height = height;
	}
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


Mesh::~Mesh()
{
	RELEASE_ARRAY(vertices);

	RELEASE_ARRAY(normals);

	RELEASE_ARRAY(texCoords);

	glDeleteBuffers(1, &id_indices);
	RELEASE_ARRAY(indices);

	glDeleteTextures(1, &texture);

	RELEASE(App->camera->BBtoLook);
}

void Mesh::Draw()
{
	//Enable
	glEnableClientState(GL_VERTEX_ARRAY);
	if (texture > 0)
		glBindTexture(GL_TEXTURE_2D, texture);
	else
		glColor3f(color.x, color.y, color.z);	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	//Draw
	glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, &texCoords[0]);
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);
	//Disable
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	if (texture > 0)
		glBindTexture(GL_TEXTURE_2D, 0);
	else
		glColor3f(1, 1, 1);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if (App->renderer3D->Normals)
	{
		glLineWidth(2.0f);
		glColor3f(0, 0.5f, 1);
		
		glBegin(GL_LINES);
		for (int i = 0; i < num_vertices*3; i=i+3)
		{
			glVertex3f(vertices[i], vertices[i+1], vertices[i + 2]);
			glVertex3f(-normals[i]*3 + vertices[i], -normals[i + 1]*3 + vertices[i + 1], -normals[i + 2]*3 + vertices[i + 2]);
		}
		glEnd();

		glColor3f(1, 1, 1);
		glLineWidth(1.0f);
	}

	if (App->renderer3D->Faces)
	{
		int vert_normal=0;

		glLineWidth(2.0f);
		glColor3f(0, 0.5f, 1);

		glBegin(GL_LINES);
		for (int i = 0; i < faceNormals.size(); i = i + 6)
		{
			glVertex3f(faceNormals[i], faceNormals[i + 1], faceNormals[i + 2]);
			glVertex3f(faceNormals[i+3] + faceNormals[i], faceNormals[i+4] + faceNormals[i + 1], faceNormals[i+5] + faceNormals[i + 2]);
			vert_normal += 9;
		}
		glEnd();

		glColor3f(1, 1, 1);
		glLineWidth(2.0f);
	}
	if (App->renderer3D->ShowBB)
	{
		glLineWidth(2.0f);
		glColor3f(0, 0.5f, 1);

		glBegin(GL_LINES);
		
		glVertex3f(boundingBox.CornerPoint(0).x, boundingBox.CornerPoint(0).y, boundingBox.CornerPoint(0).z);
		glVertex3f(boundingBox.CornerPoint(1).x, boundingBox.CornerPoint(1).y, boundingBox.CornerPoint(1).z);

		glVertex3f(boundingBox.CornerPoint(0).x, boundingBox.CornerPoint(0).y, boundingBox.CornerPoint(0).z);
		glVertex3f(boundingBox.CornerPoint(2).x, boundingBox.CornerPoint(2).y, boundingBox.CornerPoint(2).z);

		glVertex3f(boundingBox.CornerPoint(0).x, boundingBox.CornerPoint(0).y, boundingBox.CornerPoint(0).z);
		glVertex3f(boundingBox.CornerPoint(4).x, boundingBox.CornerPoint(4).y, boundingBox.CornerPoint(4).z);

		glVertex3f(boundingBox.CornerPoint(3).x, boundingBox.CornerPoint(3).y, boundingBox.CornerPoint(3).z);
		glVertex3f(boundingBox.CornerPoint(1).x, boundingBox.CornerPoint(1).y, boundingBox.CornerPoint(1).z);

		glVertex3f(boundingBox.CornerPoint(3).x, boundingBox.CornerPoint(3).y, boundingBox.CornerPoint(3).z);
		glVertex3f(boundingBox.CornerPoint(2).x, boundingBox.CornerPoint(2).y, boundingBox.CornerPoint(2).z);

		glVertex3f(boundingBox.CornerPoint(3).x, boundingBox.CornerPoint(3).y, boundingBox.CornerPoint(3).z);
		glVertex3f(boundingBox.CornerPoint(7).x, boundingBox.CornerPoint(7).y, boundingBox.CornerPoint(7).z);

		glVertex3f(boundingBox.CornerPoint(6).x, boundingBox.CornerPoint(6).y, boundingBox.CornerPoint(6).z);
		glVertex3f(boundingBox.CornerPoint(2).x, boundingBox.CornerPoint(2).y, boundingBox.CornerPoint(2).z);

		glVertex3f(boundingBox.CornerPoint(6).x, boundingBox.CornerPoint(6).y, boundingBox.CornerPoint(6).z);
		glVertex3f(boundingBox.CornerPoint(4).x, boundingBox.CornerPoint(4).y, boundingBox.CornerPoint(4).z);

		glVertex3f(boundingBox.CornerPoint(6).x, boundingBox.CornerPoint(6).y, boundingBox.CornerPoint(6).z);
		glVertex3f(boundingBox.CornerPoint(7).x, boundingBox.CornerPoint(7).y, boundingBox.CornerPoint(7).z);

		glVertex3f(boundingBox.CornerPoint(5).x, boundingBox.CornerPoint(5).y, boundingBox.CornerPoint(5).z);
		glVertex3f(boundingBox.CornerPoint(1).x, boundingBox.CornerPoint(1).y, boundingBox.CornerPoint(1).z);

		glVertex3f(boundingBox.CornerPoint(5).x, boundingBox.CornerPoint(5).y, boundingBox.CornerPoint(5).z);
		glVertex3f(boundingBox.CornerPoint(4).x, boundingBox.CornerPoint(4).y, boundingBox.CornerPoint(4).z);

		glVertex3f(boundingBox.CornerPoint(5).x, boundingBox.CornerPoint(5).y, boundingBox.CornerPoint(5).z);
		glVertex3f(boundingBox.CornerPoint(7).x, boundingBox.CornerPoint(7).y, boundingBox.CornerPoint(7).z);
		
		glEnd();

		glColor3f(1, 1, 1);
		glLineWidth(1.0f);

		App->camera->renderTotalBB();
	}
}

void Mesh::calculateNormals()
{
	for (int i = 0; i < num_indices; i += 3)
	{
		float x, y, z;
		vec3 A, B, C, N;

		A = {vertices[indices[i] * 3],vertices[indices[i] * 3 + 1],vertices[indices[i] * 3 + 2] };
		B = {vertices[indices[i + 1] * 3],vertices[indices[i + 1] * 3 + 1],vertices[indices[i + 1] * 3 + 2] };
		C = {vertices[indices[i + 2] * 3],vertices[indices[i + 2] * 3 + 1],vertices[indices[i + 2] * 3 + 2] };
		N = cross(B - A, C - A);
		N = normalize(N);

		x = (vertices[indices[i] * 3] + vertices[indices[i + 1] * 3] + vertices[indices[i + 2] * 3]) / 3;
		y = (vertices[indices[i] * 3 + 1] + vertices[indices[i + 1] * 3 + 1] + vertices[indices[i + 2] * 3 + 1]) / 3;
		z = (vertices[indices[i] * 3 + 2] + vertices[indices[i + 1] * 3 + 2] + vertices[indices[i + 2] * 3 + 2]) / 3;

		faceNormals.push_back(x);
		faceNormals.push_back(y);
		faceNormals.push_back(z);
		faceNormals.push_back(N.x);
		faceNormals.push_back(N.y);
		faceNormals.push_back(N.z);


	}
}

