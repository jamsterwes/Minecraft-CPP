#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <SOIL2\SOIL2.h>
#include <initializer_list>

namespace gfx
{
	struct texture2D
	{
		unsigned int textureID;
		const char* texturePath;

		texture2D(const char* filePath) : texturePath(filePath) {}
		void load(bool repeat = false);
		void slot(GLenum slot);
	};

	struct textureCube
	{
		unsigned int textureID;
		const char** texturePaths;

		textureCube(const char* texturePaths[6]) : texturePaths(texturePaths) { }
		void load();
		void slot(GLenum slot);
	};
}