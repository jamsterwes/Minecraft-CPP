#pragma once
#include <glad/glad.h>
#include "../gfx/shaders.hpp"

namespace lighting
{
    class DeferredRenderer
    {
    public:
        DeferredRenderer(int width, int height);

        void Resize(int width, int height);
        void Use(gfx::shader& shaderLighting);
    private:
        void CreateTexture(unsigned int tex, int width, int height, GLint internalFormat, GLenum format, GLenum type, GLenum attachment);

        unsigned int gBuffer;
        unsigned int gPosition, gNormal, gAlbedoSpec;
    };
}