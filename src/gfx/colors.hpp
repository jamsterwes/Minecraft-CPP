#pragma once
#include <glad\glad.h>
#include <string>

#define UNPACK_COLOR3(color) color.r, color.g, color.b
#define UNPACK_COLOR4(color) color.r, color.g, color.b, color.a

namespace gfx
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