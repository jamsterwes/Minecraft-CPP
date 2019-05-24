#pragma once
#include <glad/glad.h>
#include "settings.hpp"
#include "../gfx/camera.hpp"
#include "../gfx/iv_buffer.hpp"
#include "../gfx/shaders.hpp"
#include "../gfx/textures.hpp"
#include "../post/fs_quad.hpp"
#include "shadow_cam.hpp"
#include "ssao.hpp"

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
        
        void ShadowMapPass(minecraft::WorldRenderer& world, ShadowCam& cam);

        gfx::shader* lightingShader;

        // Effects
        SSAO* ssao;
    private:
        void CreateTexture(unsigned int tex, int width, int height, GLint internalFormat, GLenum format, GLenum type, GLenum attachment);
        void GBufferPass(minecraft::WorldRenderer& world);
        void SSAOPass(gfx::camera& cam);
        void SSAOBlurPass();

        // G-Buffer
        unsigned int gBuffer;
        unsigned int gPosition, gNormal, gAlbedoSpec, gDepthStencil;
        int width, height;

        // Instanced Rendering
        gfx::shader* gBufferShader;
        gfx::iv_buffer<>* cubeModel;
        gfx::texture2D* atlas;
        post::FSQuad* quad;
    };
}