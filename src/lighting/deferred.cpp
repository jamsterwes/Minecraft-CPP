#include "deferred.hpp"
#include "../models/cube.hpp"
#include "../minecraft/world_renderer.hpp"
#include <glm/gtx/transform.hpp>

namespace lighting
{
    DeferredRenderer::DeferredRenderer(int width, int height)
    {
        // Init Instanced Rendering
        gBufferShader = new gfx::shader("shaders/deferred.vert", "shaders/deferred.frag");
        gBufferShader->compile();
        lightingShader = new gfx::shader("shaders/lighting.vert", "shaders/lighting.frag");
        lightingShader->compile();
        cubeModel = new gfx::iv_buffer<>(cube_data::indices, ARRAYSIZE(cube_data::indices),
                                            cube_data::vertices, ARRAYSIZE(cube_data::vertices), []() {
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(5 * sizeof(float)));
            glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glEnableVertexAttribArray(5);
        });
        atlas = new gfx::texture2D("res/atlas.png");
        atlas->load();
        quad = new post::FSQuad();

        // Create G-Buffer
        glGenFramebuffers(1, &gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glGenTextures(1, &gPosition);
        glGenTextures(1, &gNormal);
        glGenTextures(1, &gAlbedoSpec);
        glGenRenderbuffers(1, &gDepthStencil);
        Resize(width, height);
        // Attach G-Buffer sub-buffers
        unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    DeferredRenderer::~DeferredRenderer()
    {
        delete gBufferShader, lightingShader, cubeModel, atlas, quad;
    }

    unsigned int DeferredRenderer::GetVAO()
    {
        return cubeModel->vaoID;
    }

    void DeferredRenderer::ClearGBuffer()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void DeferredRenderer::RenderWorld(minecraft::WorldRenderer& world, gfx::camera& cam)
    {
        cam.recalculate(gBufferShader);
        GBufferPass(world);
    }
    
    void DeferredRenderer::RenderToScreen(gfx::camera& cam, LightingSettings lightSettings)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        lightingShader->use();
        lightingShader->setTexture("gPosition", 0);
        lightingShader->setTexture("gNormal", 1);
        lightingShader->setTexture("gAlbedoSpec", 2);
        lightingShader->setVector("LightDir", lightSettings.LightDir);
        lightingShader->setColor("LightColor", lightSettings.LightColor);
        lightingShader->setFloat("FogDensity", lightSettings.FogDensity);
        lightingShader->setColor("FogColor", lightSettings.FogColor);
        cam.recalculate(lightingShader);
        quad->Draw();
    }

    void DeferredRenderer::Resize(int width, int height)
    {
        CreateTexture(gPosition, width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0);
        CreateTexture(gNormal, width, height, GL_RGB16F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT1);
        CreateTexture(gAlbedoSpec, width, height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT2);
    
        glBindRenderbuffer(GL_RENDERBUFFER, gDepthStencil);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gDepthStencil);
    }

    void DeferredRenderer::GBufferPass(minecraft::WorldRenderer& world)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glDepthMask(true);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);

        gBufferShader->use();
        glm::mat4 model = glm::mat4(1.0f);
        gBufferShader->setMatrix("model", model);
        gBufferShader->setTexture("Atlas", 0);

        atlas->slot(GL_TEXTURE0);
        world.LinkToRenderer(*this);
        cubeModel->DrawInstanced(GL_UNSIGNED_INT, world.GetInstanceCount());

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void DeferredRenderer::CreateTexture(unsigned int tex, int width, int height, GLint internalFormat, GLenum format, GLenum type, GLenum attachment)
    {
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tex, 0);
    }
}