#include "Application.h"
#include "ModuleRenderer3D.h"
#include "GameObject.h"
#include "Component.h"
#include "ComponentTransformation.h"
#include "ComponentTexture.h"
#include "ComponentMesh.h"


GameObject::GameObject()
{
}


GameObject::~GameObject()
{
}

void GameObject::Update()
{

}

void GameObject::Draw()
{
	for (std::list<Component*>::iterator it_c=components.begin();it_c!=components.end();it_c++)
	{
		if ((*it_c)->type == MESH && (*it_c)->active)
		{
			Mesh* mesh = ((ComponentMesh*)(*it_c))->mesh;
			
			if (mesh->texture > 0)
				glBindTexture(GL_TEXTURE_2D, mesh->texture);
			else
				glColor3f(mesh->color.x, mesh->color.y, mesh->color.z);
			
			//Assign Vertices
			glEnableClientState(GL_VERTEX_ARRAY);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_indices);
			glVertexPointer(3, GL_FLOAT, 0, &mesh->vertices[0]);
			
			//Assign texture
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			if (mesh->texCoords != nullptr)
				glTexCoordPointer(2, GL_FLOAT, 0, &mesh->texCoords[0]);
			
			//Draw
			glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT, NULL);
			
			//Disable
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			if (mesh->texture > 0)
				glBindTexture(GL_TEXTURE_2D, 0);
			else
				glColor3f(1, 1, 1);
			
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

			if (App->renderer3D->Normals && mesh->normals != nullptr)
			{
				glLineWidth(2.0f);
				glColor3f(0, 0.5f, 1);

				glBegin(GL_LINES);
				for (int i = 0; i < mesh->num_vertices * 3; i = i + 3)
				{
					glVertex3f(mesh->vertices[i], mesh->vertices[i + 1], mesh->vertices[i + 2]);
					glVertex3f(-mesh->normals[i] * 3 + mesh->vertices[i], -mesh->normals[i + 1] * 3 + mesh->vertices[i + 1], -mesh->normals[i + 2] * 3 + mesh->vertices[i + 2]);
				}
				glEnd();

				glColor3f(1, 1, 1);
				glLineWidth(1.0f);
			}

			if (App->renderer3D->Faces && mesh->faceNormals.size() > 0)
			{
				int vert_normal = 0;

				glLineWidth(2.0f);
				glColor3f(0, 0.5f, 1);

				glBegin(GL_LINES);
				for (int i = 0; i < mesh->faceNormals.size(); i = i + 6)
				{
					glVertex3f(mesh->faceNormals[i], mesh->faceNormals[i + 1], mesh->faceNormals[i + 2]);
					glVertex3f(mesh->faceNormals[i + 3] + mesh->faceNormals[i], mesh->faceNormals[i + 4] + mesh->faceNormals[i + 1], mesh->faceNormals[i + 5] + mesh->faceNormals[i + 2]);
					vert_normal += 9;
				}
				glEnd();

				glColor3f(1, 1, 1);
				glLineWidth(2.0f);
			}
			if (App->renderer3D->ShowBB)
			{
				App->renderer3D->DrawBB(mesh->boundingBox, { 0, 0.5f, 1 });
			}
		}

	}
}

Component* GameObject::AddComponent(componentType type)
{
	Component* ret;

	switch (type)
	{
	case TRANSFORMATION:
		ret = new ComponentTransformation(this,type);
		components.push_back(ret);
		break;
	case MESH:
		ret=new ComponentMesh(this, type);
		components.push_back(ret);
		break;
	case MATERIAL:
		ret = new ComponentTexture(this, type);
		components.push_back(ret);
		break;
	}

	return ret;
}

void GameObject::RemoveComponent(Component& component)
{

}

ComponentTransformation* GameObject::CreateComponentTransformation()
{
	ComponentTransformation* ret;

	return ret;
}