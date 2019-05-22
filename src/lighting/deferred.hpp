#pragma once
#include <glad/glad.h>
#include "settings.hpp"
#include "../gfx/camera.hpp"
#include "../gfx/iv_buffer.hpp"
#include "../gfx/shaders.hpp"
#include "../gfx/textures.hpp"

namespace minecraft
{
    class Chunk;
}

namespace lighting
{
    class DeferredRenderer
    {
    public:
        DeferredRenderer(int width, int height);
        ~DeferredRenderer();

        unsigned int GetVAO();
        void RenderChunk(minecraft::Chunk& chunk, gfx::camera& cam, LightingSettings lightSettings);
        void Resize(int width, int height);
    private:
        void CreateTexture(unsigned int tex, int width, int height, GLint internalFormat, GLenum format, GLenum type, GLenum attachment);
        void GBufferPass(minecraft::Chunk& chunk);
        void LightingPass(LightingSettings lightSettings);

        // G-Buffer
        unsigned int gBuffer;
        unsigned int gPosition, gNormal, gAlbedoSpec, gDepthStencil;

        // Instanced Rendering
        gfx::shader* gBufferShader;
        gfx::shader* lightingShader;
        gfx::iv_buffer<>* cubeModel;
        gfx::texture2D* atlas;
    };
}