#include "Application.h"
#include "Globals.h"
#include "Primitive.h"
#include "ModuleScene.h"
#include "ModuleResource.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "Resource.h"
#include "ResourceMesh.h"
#include "Glew/include/glew.h"
#include "SDL\include\SDL_opengl.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Glew/libx86/glew32.lib") /* link Microsoft OpenGL lib   */

// ------------------------------------------------------------
Primitive::Primitive() :  color(White), wire(false), axis(false), type(PrimitiveTypes::Primitive_Point)
{
	transform.SetIdentity();
}

Primitive::~Primitive()
{
	glDeleteBuffers(1, &my_id);
	shape.clear();
	indices.clear();
}

// ------------------------------------------------------------
PrimitiveTypes Primitive::GetType() const
{
	return type;
}

void Primitive::generateBuffer()
{
	glGenBuffers(1, (GLuint*)&(my_id));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// ------------------------------------------------------------
void Primitive::Render() const
{
	if (axis == true)
	{
		// Draw Axis Grid
		glLineWidth(2.0f);

		glBegin(GL_LINES);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
		glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
		glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
		glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

		glEnd();

		glLineWidth(1.0f);
	}

	glColor3f(color.r, color.g, color.b);

	glPushMatrix();
	glMultMatrixf((float*)transform.Transposed().v);

	//Draw shape
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_id);
	glVertexPointer(3, GL_FLOAT, 0, &shape[0]);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
}

// ------------------------------------------------------------
void Primitive::SetPos(float x, float y, float z)
{
	transform.Set(float4x4::FromTRS({ x,y,z }, Quat::identity, float3::one));
}

// ------------------------------------------------------------
void Primitive::SetRotation(float angle, const vec &u)
{
	transform.RotateAxisAngle(u, angle);
}

// ------------------------------------------------------------
void Primitive::Scale(float x, float y, float z)
{
	transform.Scale(x, y, z);
}

// CUBE ============================================
MCube::MCube() : Primitive(), size(1.0f, 1.0f, 1.0f)
{
	type = PrimitiveTypes::Primitive_Cube;
}

MCube::MCube(float sizeX, float sizeY, float sizeZ, vec center) : Primitive(), size(sizeX, sizeY, sizeZ)
{
	float sx = size.x * 0.5f;
	float sy = size.y * 0.5f;
	float sz = size.z * 0.5f;

	//uint *indices = new uint[36];
	uint aux[36]= { 0,1,2,1,3,2,3,1,5,5,7,3,7,5,4,6,7,4,6,4,0,0,2,6,6,2,3,6,3,7,0,4,5,0,5,1 };

	//for (int i = 0; i < 36; i++)
	//{
	//	indices[i] = aux[i];
	//}


	shape.push_back(center.x - sx); shape.push_back(center.y - sy); shape.push_back(center.z + sz);	//A
	shape.push_back(center.x + sx); shape.push_back(center.y - sy); shape.push_back(center.z + sz);	//B
	shape.push_back(center.x - sx); shape.push_back(center.y + sy); shape.push_back(center.z + sz);	//C
	shape.push_back(center.x + sx); shape.push_back(center.y + sy); shape.push_back(center.z + sz);	//D
																						   
	shape.push_back(center.x - sx); shape.push_back(center.y - sy); shape.push_back(center.z - sz);	//E
	shape.push_back(center.x + sx); shape.push_back(center.y - sy); shape.push_back(center.z - sz);	//F
	shape.push_back(center.x - sx); shape.push_back(center.y + sy); shape.push_back(center.z - sz);	//G
	shape.push_back(center.x + sx); shape.push_back(center.y + sy); shape.push_back(center.z - sz);	//H

	//float* vertices = new float[shape.size()];

	//for (int i = 0; i < shape.size(); i++)
	//{
	//	vertices[i] = shape[i];
	//}

	indices.push_back(0); indices.push_back(1); indices.push_back(2);
	indices.push_back(1); indices.push_back(3); indices.push_back(2);

	indices.push_back(3); indices.push_back(1); indices.push_back(5);
	indices.push_back(5); indices.push_back(7); indices.push_back(3);

	indices.push_back(7); indices.push_back(5); indices.push_back(4);
	indices.push_back(6); indices.push_back(7); indices.push_back(4);

	indices.push_back(6); indices.push_back(4); indices.push_back(0);
	indices.push_back(0); indices.push_back(2); indices.push_back(6);

	indices.push_back(6); indices.push_back(2); indices.push_back(3);
	indices.push_back(6); indices.push_back(3); indices.push_back(7);

	indices.push_back(0); indices.push_back(4); indices.push_back(5);
	indices.push_back(0); indices.push_back(5); indices.push_back(1);

	generateBuffer();

	/*GameObject* newGameObject = new GameObject();
	newGameObject->name = "Box";
	App->scene_intro->addGameObject(newGameObject);

	newGameObject->AddComponent(TRANSFORMATION);
	ComponentMesh* mesh =(ComponentMesh*)newGameObject->AddComponent(MESH);
	ResourceMesh* newResource = (ResourceMesh*) App->resources->AddResource(R_MESH);
	mesh->RUID = newResource->GetUID();

	newResource->num_indices=36;
	newResource->num_vertices = 8;
	newResource->indices = indices;
	newResource->vertices = vertices;

	glGenBuffers(1, (GLuint*)&(newResource->id_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newResource->id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * newResource->num_indices, &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
*/
	type = PrimitiveTypes::Primitive_Cube;
}

// SPHERE ============================================
MSphere::MSphere() : MSphere(1.0f, 6, 12, { 0,0,0 })
{
}

MSphere::MSphere(float radius, int rings, int sectors, vec pos) : Primitive(), radius(radius)
{
	type = PrimitiveTypes::Primitive_Sphere;

	//TEMP
	vec initialPos = pos;

	float x, y, z, xz;                              // vertex position

	float sectorStep = 2 * pi / sectors;
	float stackStep = pi / rings;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= rings; ++i)
	{
		stackAngle = pi / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xz = radius * cosf(stackAngle);             // r * cos(u)
		y = initialPos.y + radius * sinf(stackAngle);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coods
		for (int j = 0; j <= sectors; ++j)
		{
			sectorAngle = j * sectorStep;

			// vertex position (x, y, z)
			z = initialPos.z + xz * cosf(sectorAngle);             // r * cos(u) * cos(v)
			x = initialPos.x + xz * sinf(sectorAngle);             // r * cos(u) * sin(v)
			shape.push_back(x);
			shape.push_back(y);
			shape.push_back(z);
		}
	}

	int k1, k2;
	for (int i = 0; i < rings; ++i)
	{
		k1 = i * (sectors + 1);     // beginning of current stack
		k2 = k1 + sectors + 1;      // beginning of next stack

		for (int j = 0; j < sectors; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding 1st and last stacks
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			if (i != (rings - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	generateBuffer();
}

// CYLINDER ============================================
MCylinder::MCylinder() : Primitive(), radius(1.0f), height(1.0f)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

MCylinder::MCylinder(float radius, float heigh, int rings, int sectors,vec pos) : Primitive(), radius(radius), height(height)
{
	vec initialPos = pos;

	float x, y, z, xz;                              // vertex position

	float sectorStep = 2 * pi / sectors;
	float stackStep = pi / rings;
	float sectorAngle, stackAngle;
	float rise = heigh / rings;

	for (int i = 0; i <= rings; ++i)
	{
		stackAngle = pi / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		y = initialPos.y + rise*i;              // r * sin(u)
																   // add (sectorCount+1) vertices per stack
																   // the first and last vertices have same position and normal, but different tex coods
		for (int j = 0; j <= sectors; ++j)
		{
			sectorAngle = j * sectorStep;

			// vertex position (x, y, z)
			z = initialPos.z + radius * cosf(sectorAngle);             // r * cos(u) * cos(v)
			x = initialPos.x + radius * sinf(sectorAngle);             // r * cos(u) * sin(v)
			shape.push_back(x);
			shape.push_back(y);
			shape.push_back(z);
		}
	}

	shape.push_back(initialPos.x);
	shape.push_back(initialPos.y);
	shape.push_back(initialPos.z);

	LOG("%f %f %f", initialPos.x, initialPos.y, initialPos.z);

	shape.push_back(initialPos.x);
	shape.push_back(initialPos.y + heigh);
	shape.push_back(initialPos.z);
	

	int k1, k2;

	//bottom circle;
	for (int i = 0; i < sectors; i++)
	{
		indices.push_back(i);
		indices.push_back(((int)shape.size() / 3) - 2);
		indices.push_back(i+1);
	}

	//Tube
	for (int i = 0; i < rings; ++i)
	{
		k1 = i * (sectors + 1);     // beginning of current stack
		k2 = k1 + sectors + 1;      // beginning of next stack

		for (int j = 0; j < sectors; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding 1st and last stacks
			if (i >= 0)
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k1);
				
			}

			if (i != rings)
			{
				indices.push_back(k2 + 1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}
		}
	}
	
	//Bottom Circle
	int last_vertex = (shape.size()/3)-sectors-3;
	for (int i = 0; i < sectors; i++)
	{
		indices.push_back(((int)shape.size() / 3) - 1);
		indices.push_back(i+ last_vertex);
		indices.push_back(i + last_vertex + 1);
	}

	generateBuffer();


	type = PrimitiveTypes::Primitive_Cylinder;
}

// LINE ==================================================
MLine::MLine() : Primitive(), origin(0, 0, 0), destination(1, 1, 1)
{
	type = PrimitiveTypes::Primitive_Line;
}

MLine::MLine(float x, float y, float z, vec origin) : Primitive(), origin(origin.x, origin.y, origin.z), destination(x, y, z)
{
	type = PrimitiveTypes::Primitive_Line;
}

void MLine::Render() const
{
	glLineWidth(2.0f);

	glColor3f(color.r, color.g, color.b);
	glBegin(GL_LINES);

	glVertex3f(origin.x, origin.y, origin.z);
	glVertex3f(destination.x, destination.y, destination.z);

	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);

	glLineWidth(1.0f);
}

// PLANE ==================================================
MPlane::MPlane() : Primitive(), normal(0, 1, 0), constant(1)
{
	type = PrimitiveTypes::Primitive_Plane;
}

MPlane::MPlane(float x, float y, float z, float d) : Primitive(), normal(x, y, z), constant(d)
{
	type = PrimitiveTypes::Primitive_Plane;
}

void MPlane::Render() const
{
	//TEMP -----------------------------------------------
	if (axis == true)
	{
		// Draw Axis Grid
		glLineWidth(2.0f);

		glBegin(GL_LINES);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
		glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
		glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
		glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

		glEnd();

		glLineWidth(1.0f);
	}

	glColor3f(color.r, color.g, color.b);
	//----------------------------------------------------

	glLineWidth(1.0f);
	glBegin(GL_LINES);

	float d = 200.0f;
	for (float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}

	glEnd();
}

MArrow::MArrow() : Primitive(), origin(0, 0, 0), destination(1, 1, 1)
{
	type = PrimitiveTypes::Primitive_Line;
}

MArrow::MArrow(float x, float y, float z, vec pos) : Primitive(), origin(pos.x, pos.y, pos.z), destination(x, y, z)
{
	type = PrimitiveTypes::Primitive_Line;

	vec Y{ 0.0f,1.0f,0.0f }; //Vertical Axis
	vec direction = destination - origin;
	vec rotAxis = Cross(direction, Y); //Get the rotation axis of the head
	rotAxis.Normalize();
	float3x3 rotMatrix;

	rotMatrix = rotMatrix.RotateAxisAngle(rotAxis, 30 * pi / 180); //Get rotation matrxi for head 1
	arrowHead1 = rotMatrix*direction;
	arrowHead1 = arrowHead1.Normalized()*defaultHeadLength;
	arrowHead1 = destination - arrowHead1; //Get the vertex position

	rotMatrix = rotMatrix.RotateAxisAngle(rotAxis, -30 * pi / 180); //Get rotation matrxi for head 2
	arrowHead2 = rotMatrix*direction;
	arrowHead2 = arrowHead2.Normalized()*defaultHeadLength;
	arrowHead2 = destination - arrowHead2; //Get the vertex position
}

void MArrow::Render() const
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	glVertex3f(origin.x, origin.y, origin.z);
	glVertex3f(destination.x, destination.y, destination.z);

	glVertex3f(destination.x, destination.y, destination.z);
	glVertex3f(arrowHead1.x, arrowHead1.y, arrowHead1.z);

	glVertex3f(destination.x, destination.y, destination.z);
	glVertex3f(arrowHead2.x, arrowHead2.y, arrowHead2.z);

	glEnd();

	glLineWidth(1.0f);
}

MAxis::MAxis(): Primitive(), origin(0,0,0), size(1)
{
	type = PrimitiveTypes::Primitive_Axis;
}

MAxis::MAxis(float size, vec pos): Primitive(), origin(pos.x, pos.y, pos.z), size(size)
{
	type = PrimitiveTypes::Primitive_Axis;
}

void MAxis::Render() const
{
	// Draw Axis Grid
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

	glVertex3f(origin.x, origin.y, origin.z); glVertex3f(origin.x+size, origin.y, origin.z);
	glVertex3f(origin.x+size, origin.y+0.1f*size, origin.z); glVertex3f(origin.x+1.1f*size, origin.y-0.1f*size, origin.z);
	glVertex3f(origin.x+1.1f*size, origin.y+0.1f*size, origin.z); glVertex3f(origin.x+size, origin.y-0.1f*size, origin.z);

	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

	glVertex3f(origin.x, origin.y, origin.z); glVertex3f(origin.x, origin.y+size, origin.z);
	glVertex3f(origin.x-0.05f*size, origin.y+1.25f*size, origin.z); glVertex3f(origin.x, origin.y+1.15f*size, origin.z);
	glVertex3f(origin.x+0.05f*size, origin.y+1.25f*size, origin.z); glVertex3f(origin.x, origin.y+1.15f*size, origin.z);
	glVertex3f(origin.x, origin.y+1.15f*size, origin.z); glVertex3f(origin.x, origin.y+1.05f*size, origin.z);

	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

	glVertex3f(origin.x, origin.y, origin.z); glVertex3f(origin.x, origin.y, origin.z+size);
	glVertex3f(origin.x-0.05f*size, origin.y+0.1f*size, origin.z+1.05f*size); glVertex3f(origin.x+0.05f*size, origin.y+0.1f*size, origin.z+1.05f*size);
	glVertex3f(origin.x+0.05f*size, origin.y+0.1f*size, origin.z+1.05f*size); glVertex3f(origin.x-0.05f*size, origin.y-0.1f*size, origin.z+1.05f*size);
	glVertex3f(origin.x-0.05f*size, origin.y-0.1f*size, origin.z+1.05f*size); glVertex3f(origin.x+0.05f*size, origin.y-0.1f*size, origin.z+1.05f*size);

	glEnd();

	glLineWidth(1.0f);
}

MCapsule::MCapsule()
{
	type = PrimitiveTypes::Primitive_Capsule;
}

MCapsule::MCapsule(float radius, float height, int rings, int sectors, vec pos)
{
	type = PrimitiveTypes::Primitive_Capsule;

	float x, y, z, xz;                              // vertex position

	float sectorStep = 2 * pi / sectors;
	float stackStep = pi / rings;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= rings; ++i)
	{
		stackAngle = pi / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xz = radius * cosf(stackAngle);             // r * cos(u)
		y = pos.y + radius * sinf(stackAngle);              // r * sin(u)

		if (stackAngle < 0)
			y -= height / 2;
		else
			y += height / 2;

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coods
		for (int j = 0; j <= sectors; ++j)
		{
			sectorAngle = j * sectorStep;

			// vertex position (x, y, z)
			z = pos.z + xz * cosf(sectorAngle);             // r * cos(u) * cos(v)
			x = pos.x + xz * sinf(sectorAngle);             // r * cos(u) * sin(v)
			shape.push_back(x);
			shape.push_back(y);
			shape.push_back(z);
		}
	}

	int k1, k2;
	for (int i = 0; i < rings; ++i)
	{
		k1 = i * (sectors + 1);     // beginning of current stack
		k2 = k1 + sectors + 1;      // beginning of next stack

		for (int j = 0; j < sectors; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding 1st and last stacks
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			if (i != (rings - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	generateBuffer();
}

MFrustum::MFrustum() : distance(1.0f), depth(5.0f), width(5.0f), height(3.0f), center({0.0f,0.0f,0.0f})
{
	type = PrimitiveTypes::Primitive_Frustum;
}

MFrustum::MFrustum(float distance, float depth, float width, float height, vec center): distance(distance), depth(depth), width(width), height(height), center(center)
{
	type = PrimitiveTypes::Primitive_Frustum;

	float frontHalfWidth = ((width / 2) / distance)*(distance + depth);
	float frontHalfHeight = ((height / 2) / distance)*(distance + depth);

	shape.push_back(center.x - width / 2); shape.push_back(center.y + height / 2); shape.push_back(center.z - distance); //A(0)
	shape.push_back(center.x + width / 2); shape.push_back(center.y + height / 2); shape.push_back(center.z - distance); //B(1)
	shape.push_back(center.x - width / 2); shape.push_back(center.y - height / 2); shape.push_back(center.z - distance); //C(2)
	shape.push_back(center.x + width / 2); shape.push_back(center.y - height / 2); shape.push_back(center.z - distance); //D(3)

	shape.push_back(center.x - frontHalfWidth); shape.push_back(center.y + frontHalfHeight); shape.push_back(center.z - (distance+depth)); //E(4)
	shape.push_back(center.x + frontHalfWidth); shape.push_back(center.y + frontHalfHeight); shape.push_back(center.z - (distance+depth)); //F(5)
	shape.push_back(center.x - frontHalfWidth); shape.push_back(center.y - frontHalfHeight); shape.push_back(center.z - (distance+depth)); //G(6)
	shape.push_back(center.x + frontHalfWidth); shape.push_back(center.y - frontHalfHeight); shape.push_back(center.z - (distance+depth)); //H(7)

	indices.push_back(0); indices.push_back(2); indices.push_back(1);
	indices.push_back(1); indices.push_back(2); indices.push_back(3);

	indices.push_back(1); indices.push_back(3); indices.push_back(7);
	indices.push_back(5); indices.push_back(1); indices.push_back(7);

	indices.push_back(0); indices.push_back(1); indices.push_back(4);
	indices.push_back(4); indices.push_back(1); indices.push_back(5);

	indices.push_back(0); indices.push_back(6); indices.push_back(2);
	indices.push_back(0); indices.push_back(4); indices.push_back(6);

	indices.push_back(2); indices.push_back(6); indices.push_back(3);
	indices.push_back(3); indices.push_back(6); indices.push_back(7);

	indices.push_back(4); indices.push_back(7); indices.push_back(6);
	indices.push_back(4); indices.push_back(5); indices.push_back(7);

	generateBuffer();
}


