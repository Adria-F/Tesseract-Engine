#include "Application.h"
#include "ResourceTexture.h"



ResourceTexture::ResourceTexture(uint UID, ResType type):Resource(UID,type)
{
}


ResourceTexture::~ResourceTexture()
{
	glDeleteTextures(1, &GL_id);
}
