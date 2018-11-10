#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "ModuleMeshes.h"

#include "Resource.h"
#include "ResourceMesh.h"

ComponentMesh::ComponentMesh(GameObject* parent, componentType type) : Component(parent, type)
{
	mesh = nullptr;
	rMesh = nullptr;
}

ComponentMesh::~ComponentMesh()
{
}

bool ComponentMesh::Update()
{
	if (!active || rMesh == nullptr)
		return false;

	//Assign Vertices
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rMesh->id_indices);
	glVertexPointer(3, GL_FLOAT, 0, &rMesh->vertices[0]);

	//Assign texture
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	if (rMesh->texCoords != nullptr)
		glTexCoordPointer(2, GL_FLOAT, 0, &rMesh->texCoords[0]);

	//Draw
	glDrawElements(GL_TRIANGLES, rMesh->num_indices, GL_UNSIGNED_INT, NULL);

	//Disable
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if (App->renderer3D->Normals && rMesh->normals != nullptr)
	{
		glLineWidth(2.0f);
		glColor3f(0, 0.5f, 1);

		glBegin(GL_LINES);
		for (int i = 0; i < rMesh->num_vertices * 3; i = i + 3)
		{
			glVertex3f(rMesh->vertices[i], rMesh->vertices[i + 1], rMesh->vertices[i + 2]);
			glVertex3f(-rMesh->normals[i] * 3 + rMesh->vertices[i], -rMesh->normals[i + 1] * 3 + rMesh->vertices[i + 1], -rMesh->normals[i + 2] * 3 + rMesh->vertices[i + 2]);
		}
		glEnd();

		glColor3f(1, 1, 1);
		glLineWidth(1.0f);
	}

	if (App->renderer3D->Faces && rMesh->faceNormals.size() > 0)
	{
		int vert_normal = 0;

		glLineWidth(2.0f);
		glColor3f(0, 0.5f, 1);

		glBegin(GL_LINES);
		for (int i = 0; i < rMesh->faceNormals.size(); i = i + 6)
		{
			glVertex3f(rMesh->faceNormals[i], rMesh->faceNormals[i + 1], rMesh->faceNormals[i + 2]);
			glVertex3f(rMesh->faceNormals[i + 3] + rMesh->faceNormals[i], rMesh->faceNormals[i + 4] + rMesh->faceNormals[i + 1], rMesh->faceNormals[i + 5] + rMesh->faceNormals[i + 2]);
			vert_normal += 9;
		}
		glEnd();

		glColor3f(1, 1, 1);
		glLineWidth(2.0f);
	}

	return true;
}

void ComponentMesh::DrawInfo()
{
	ImGui::PushID("toggleMesh");
	ImGui::Checkbox("", &active);
	ImGui::PopID();
	ImGui::SameLine();
	
	if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
	{
		if (rMesh != nullptr)
		{
			ImGui::Text("Triangles Count: %d", rMesh->num_indices / 3);
			ImGui::Text("Vertices Count: %d", rMesh->num_vertices);
			ImGui::Text("Mesh size:\n X: %f | Y: %f | Z: %f", rMesh->boundingBox.Size().x, rMesh->boundingBox.Size().y, rMesh->boundingBox.Size().z);
		}
		else
		{
			ImGui::Text("No mesh attached");
		}
	}
}

void ComponentMesh::Save(JSON_Value * component) const
{
	JSON_Value* mesh = component->createValue();

	mesh->addInt("Type", type);
	mesh->addString("mesh", this->rMesh->name.c_str());

	component->addValue("", mesh);
}

void ComponentMesh::Load(JSON_Value* component)
{
	mesh = App->meshes->loadMesh(component->getString("mesh"));
}
