#include "colors.hpp"

int hexCoupleToInt(std::string src, int i)
{
	return std::stoi(src.substr(i * 2, 2), nullptr, 16);
}

namespace gfx
{
	color::color(std::string hex)
	{
		int ri, gi, bi, ai = 255;
		// HANDLE #
		if (hex[0] == '#') hex = hex.substr(1);
		// HANDLE RGB
		ri = hexCoupleToInt(hex, 0);
		gi = hexCoupleToInt(hex, 1);
		bi = hexCoupleToInt(hex, 2);
		// HANDLE ALPHA
		if (hex.length() == 8) ai = hexCoupleToInt(hex, 3);

		r = ri / 255.f;
		g = gi / 255.f;
		b = bi / 255.f;
		a = ai / 255.f;
	}

	color::color(float r, float g, float b, float a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}
}