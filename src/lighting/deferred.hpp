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
        void RenderWorld(std::function<void()> drawGBuffer, gfx::camera& cam);
        void RenderToScreen(gfx::camera& cam, LightingSettings lightSettings);
        void Resize(int width, int height);

        gfx::shader* gBufferShader;
        gfx::shader* lightingShader;

        // Effects
        SSAO* ssao;
    private:
        void CreateTexture(unsigned int tex, int width, int height, GLint internalFormat, GLenum format, GLenum type, GLenum attachment);
        void CreateTextureMS(unsigned int tex, unsigned int samples, int width, int height, GLint internalFormat, GLenum attachment);
        void GBufferPass(std::function<void()> drawGBuffer);
        void SSAOPass(gfx::camera& cam);
        void SSAOBlurPass();
        void ResolveMS();

        // G-Buffer
        unsigned int gBuffer;
        unsigned int gPosition, gNormal, gAlbedoSpec;

        // G-Buffer MS
        unsigned int gBufferMS;
        unsigned int gPositionMS, gNormalMS, gAlbedoSpecMS, gDepthStencilMS;

        int width, height;

        // Instanced Rendering
        gfx::iv_buffer<>* cubeModel;
        gfx::texture2D* atlas;
        post::FSQuad* quad;
    };
}