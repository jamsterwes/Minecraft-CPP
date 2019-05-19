#pragma once
#include <glad\glad.h>
#include <string>

namespace colors
{
	struct color
	{
		GLfloat r;
		GLfloat g;
		GLfloat b;
		GLfloat a;

		color(std::string hex);
		color(float r, float g, float b, float a = 1.0f);
	};
}