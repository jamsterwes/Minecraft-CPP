#include "shaders.hpp"
#include "util.hpp"
#include <fstream>
#include <string>
#include <glm/gtc/type_ptr.hpp>

bool verifyShader(unsigned int shaderID)
{
	int success;
	char infoLog[512];
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
		std::string errorMessage(infoLog);
		errorMessage = "Error compiling shader:\n" + errorMessage;
		ERROR_MESSAGE(errorMessage.c_str());
	}
	return success == 1;
}

bool verifyShaderProgram(unsigned int programID)
{
	int success;
	char infoLog[512];
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programID, 512, NULL, infoLog);
		std::string errorMessage(infoLog);
		errorMessage = "Error linking shader program:\n" + std::to_string(success) + "\n" + errorMessage;
		ERROR_MESSAGE(errorMessage.c_str());
	}
	return success == 1;
}

bool compileShader(unsigned int shaderID, const char* filename)
{
	std::ifstream shaderFile(filename);
	std::string shaderSource{ std::istreambuf_iterator<char>(shaderFile), std::istreambuf_iterator<char>() };
	const char* shaderSourceC = shaderSource.c_str();
	glShaderSource(shaderID, 1, &shaderSourceC, NULL);
	glCompileShader(shaderID);

	return verifyShader(shaderID);
}

namespace gfx
{
	shader::shader(const char* vertFilename, const char* fragFilename)
	{
		this->vertFilename = vertFilename;
		this->fragFilename = fragFilename;
	}

	void shader::compile()
	{
		vertID = glCreateShader(GL_VERTEX_SHADER);
		fragID = glCreateShader(GL_FRAGMENT_SHADER);

		compileShader(vertID, vertFilename);
		compileShader(fragID, fragFilename);

		progID = glCreateProgram();
		glAttachShader(progID, vertID);
		glAttachShader(progID, fragID);
		glLinkProgram(progID);
		verifyShaderProgram(progID);
	}

	void shader::use()
	{
		glUseProgram(progID);
	}

	void shader::setColor(const char* name, gfx::color color)
	{
		int loc = glGetUniformLocation(progID, name);
		// Should be using anyway...
		glUseProgram(progID);
		glUniform4f(loc, color.r, color.g, color.b, color.a);
	}

	void shader::setInt(const char* name, int i)
	{
		int loc = glGetUniformLocation(progID, name);
		// Should be using anyway...
		glUseProgram(progID);
		glUniform1i(loc, i);
	}

	void shader::setFloat(const char* name, float f)
	{
		int loc = glGetUniformLocation(progID, name);
		// Should be using anyway...
		glUseProgram(progID);
		glUniform1f(loc, f);
	}

	void shader::setTexture(const char* name, GLuint ID)
	{
		int loc = glGetUniformLocation(progID, name);
		// Should be using anyway...
		glUseProgram(progID);
		glUniform1i(loc, ID);
	}

	void shader::setVector(const char* name, glm::vec2 vec)
	{
		int loc = glGetUniformLocation(progID, name);
		// Should be using anyway...
		glUseProgram(progID);
		glUniform2fv(loc, 1, glm::value_ptr(vec));
	}

	void shader::setVector(const char* name, glm::vec3 vec)
	{
		int loc = glGetUniformLocation(progID, name);
		// Should be using anyway...
		glUseProgram(progID);
		glUniform3fv(loc, 1, glm::value_ptr(vec));
	}

	void shader::setMatrix(const char* name, glm::mat4 mat)
	{
		int loc = glGetUniformLocation(progID, name);
		// Should be using anyway...
		glUseProgram(progID);
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
	}
}