#include "Application.h"
#include "ComponentTexture.h"
#include "ModuleTextures.h"


ComponentTexture::~ComponentTexture()
{
}

bool ComponentTexture::Update()
{
	if (Material->id > 0)
		glBindTexture(GL_TEXTURE_2D, Material->id);
	else
		glColor3f(1, 1, 1);

	return true;
}

void ComponentTexture::DrawInfo()
{
}
