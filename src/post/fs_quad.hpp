#pragma once
#include "../gfx/iv_buffer.hpp"

namespace post
{
	class FSQuad
	{
    public:
        FSQuad();
        void Draw();
    private:
		gfx::iv_buffer<>* buffer;
	};
}