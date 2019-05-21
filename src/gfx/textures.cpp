#include "textures.hpp"
#include <string>

namespace gfx
{
	void texture2D::load(bool repeat)
	{
        textureID = SOIL_load_OGL_texture(texturePath, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	
	void texture2D::slot(GLenum slot)
	{
		glActiveTexture(slot);
		glBindTexture(GL_TEXTURE_2D, textureID);
	}

	void textureCube::load()
	{
		textureID = SOIL_load_OGL_cubemap(
			texturePaths[0],  // +x
			texturePaths[1],  // -x
			texturePaths[2],  // +y
			texturePaths[3],  // -y
			texturePaths[4],  // +z
			texturePaths[5],  // -z
			SOIL_LOAD_RGB,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS // | SOIL_FLAG_INVERT_Y
		);
	}

	void textureCube::slot(GLenum slot)
	{
		glActiveTexture(slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	}
}