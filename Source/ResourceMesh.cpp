#include "Application.h"
#include "ResourceMesh.h"



ResourceMesh::ResourceMesh(uint UID, ResType type) :Resource(UID, type)
{
}

ResourceMesh::~ResourceMesh()
{
	RELEASE_ARRAY(vertices);

	RELEASE_ARRAY(normals);

	RELEASE_ARRAY(texCoords);

	UnloadFromMemory();
	RELEASE_ARRAY(indices);

}

void ResourceMesh::setImportDefaults(JSON_Value & importSettings)
{
}

bool ResourceMesh::LoadInMemory()
{
	glGenBuffers(1, (GLuint*)&(id_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_indices, &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return true;
}

bool ResourceMesh::UnloadFromMemory()
{
	glDeleteBuffers(1, &id_indices);

	return true;
}

void ResourceMesh::calculateNormals()
{
	for (int i = 0; i < num_indices; i += 3)
	{
		float x, y, z;
		vec3 A, B, C, N;

		A = { vertices[indices[i] * 3],vertices[indices[i] * 3 + 1],vertices[indices[i] * 3 + 2] };
		B = { vertices[indices[i + 1] * 3],vertices[indices[i + 1] * 3 + 1],vertices[indices[i + 1] * 3 + 2] };
		C = { vertices[indices[i + 2] * 3],vertices[indices[i + 2] * 3 + 1],vertices[indices[i + 2] * 3 + 2] };
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