#pragma once
#include <glad/glad.h>
#include "settings.hpp"
#include "../gfx/camera.hpp"
#include "../gfx/iv_buffer.hpp"
#include "../gfx/shaders.hpp"
#include "../gfx/textures.hpp"
#include "../post/fs_quad.hpp"

namespace minecraft
{
    class WorldRenderer;
}

namespace lighting
{
    class DeferredRenderer
    {
    public:
        DeferredRenderer(int width, int height);
        ~DeferredRenderer();

        unsigned int GetVAO();
        void ClearGBuffer();
        void RenderWorld(minecraft::WorldRenderer& world, gfx::camera& cam);
        void RenderToScreen(gfx::camera& cam, LightingSettings lightSettings);
        void Resize(int width, int height);
    private:
        void CreateTexture(unsigned int tex, int width, int height, GLint internalFormat, GLenum format, GLenum type, GLenum attachment);
        void GBufferPass(minecraft::WorldRenderer& world);

        // G-Buffer
        unsigned int gBuffer;
        unsigned int gPosition, gNormal, gAlbedoSpec, gDepthStencil;

        // Instanced Rendering
        gfx::shader* gBufferShader;
        gfx::shader* lightingShader;
        gfx::iv_buffer<>* cubeModel;
        gfx::texture2D* atlas;
        post::FSQuad* quad;
    };
}