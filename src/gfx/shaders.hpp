#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "colors.hpp"

#include <glm/glm.hpp>

namespace gfx
{
	struct shader
	{
		const char* vertFilename;
		const char* fragFilename;
		unsigned int vertID, fragID, progID;

		shader(const char* vertFilename, const char* fragFilename);

		void compile();
		void use();

		// uniforms
		void setColor(const char* name, gfx::color color);
		void setInt(const char* name, int i);
		void setFloat(const char* name, float f);
		void setTexture(const char* name, GLuint ID);
		void setVector(const char* name, glm::vec2 vec);
		void setVector(const char* name, glm::vec3 vec);
		void setMatrix(const char* name, glm::mat4 mat);
	};
}