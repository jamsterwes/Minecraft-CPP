#include "deferred.hpp"
#include "../models/cube.hpp"
#include <glm/gtx/transform.hpp>
#include <iostream>

namespace lighting
{
    DeferredRenderer::DeferredRenderer(int width, int height) : width(width), height(height)
    {
        // Init Instanced Rendering
        gBufferShader = new gfx::shader("shaders/deferredMesh.vert", "shaders/deferred.frag");
        gBufferShader->compile();
        lightingShader = new gfx::shader("shaders/postPass.vert", "shaders/lighting.frag");
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

        // Create G-Buffer MS
        glGenFramebuffers(1, &gBufferMS);
        glBindFramebuffer(GL_FRAMEBUFFER, gBufferMS);
        glGenTextures(1, &gPositionMS);
        glGenTextures(1, &gNormalMS);
        glGenTextures(1, &gAlbedoSpecMS);
        glGenRenderbuffers(1, &gDepthStencilMS);

        Resize(width, height);
        
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "G-Buffer is not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, gBufferMS);
        unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "G-Buffer [MS] is not complete!" << std::endl;

        ssao = new SSAO(width, height);
    }

    DeferredRenderer::~DeferredRenderer()
    {
        delete gBufferShader, lightingShader, cubeModel, atlas, quad, ssao;
    }

    unsigned int DeferredRenderer::GetVAO()
    {
        return cubeModel->vaoID;
    }

    void DeferredRenderer::ClearGBuffer()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, gBufferMS);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void DeferredRenderer::RenderWorld(std::function<void()> drawGBuffer, gfx::camera& cam)
    {
        cam.recalculate(gBufferShader);
        GBufferPass(drawGBuffer);
    }
    
    void DeferredRenderer::RenderToScreen(gfx::camera& cam, LightingSettings lightSettings)
    {
        // Render SSAO pass
        SSAOPass(cam);
        SSAOBlurPass();
        // Render lighting pass
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, ssao->aoBlurBuffer);
        lightingShader->use();
        lightingShader->setTexture("gPosition", 0);
        lightingShader->setTexture("gNormal", 1);
        lightingShader->setTexture("gAlbedoSpec", 2);
        lightingShader->setTexture("gAO", 3);
        lightingShader->setVector("LightDir", lightSettings.LightDir);
        lightingShader->setColor("LightColor", lightSettings.LightColor);
        lightingShader->setFloat("FogDensity", lightSettings.FogDensity);
        lightingShader->setColor("FogColor", lightSettings.FogColor);
        cam.recalculate(lightingShader);
        quad->Draw();
    }

    void DeferredRenderer::Resize(int width, int height)
    {
        this->width = width;
        this->height = height;

        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        CreateTexture(gPosition, width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0);
        CreateTexture(gNormal, width, height, GL_RGB16F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT1);
        CreateTexture(gAlbedoSpec, width, height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT2);

        glBindFramebuffer(GL_FRAMEBUFFER, gBufferMS);
        CreateTextureMS(gPositionMS, 2, width, height, GL_RGBA16F, GL_COLOR_ATTACHMENT0);
        CreateTextureMS(gNormalMS, 2, width, height, GL_RGB16F, GL_COLOR_ATTACHMENT1);
        CreateTextureMS(gAlbedoSpecMS, 2, width, height, GL_RGBA, GL_COLOR_ATTACHMENT2);
    
        glBindRenderbuffer(GL_RENDERBUFFER, gDepthStencilMS);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 2, GL_DEPTH_COMPONENT24, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gDepthStencilMS);
    }

    void DeferredRenderer::GBufferPass(std::function<void()> drawGBuffer)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, gBufferMS);
        glDepthMask(true);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);

        gBufferShader->use();
        glm::mat4 model = glm::mat4(1.0f);
        gBufferShader->setMatrix("model", model);
        gBufferShader->setTexture("Atlas", 0);

        atlas->slot(GL_TEXTURE0);

        drawGBuffer();

        ResolveMS();
    }

    void DeferredRenderer::SSAOPass(gfx::camera& cam)
    {
        cam.recalculate(ssao->ssaoShader);
        glBindFramebuffer(GL_FRAMEBUFFER, ssao->frameBuffer);
        // glViewport(0, 0, (int)(width / 2), (int)(height / 2));
        glClear(GL_COLOR_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, ssao->noiseTex);
        ssao->ssaoShader->use();
        for (int i = 0; i < 64; i++)
        {
            std::string uniformName = "Samples[" + std::to_string(i) + "]";
            ssao->ssaoShader->setVector(uniformName.c_str(), ssao->ssaoKernel[i]);
        }
        // ssao->ssaoShader->setVector("ScreenSize", glm::vec2((int)(width / 2), (int)(height / 2)));
        ssao->ssaoShader->setVector("ScreenSize", glm::vec2(width, height));
        ssao->ssaoShader->setTexture("gPosition", 0);
        ssao->ssaoShader->setTexture("gNormal", 1);
        ssao->ssaoShader->setTexture("texNoise", 2);
        quad->Draw();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glViewport(0, 0, width, height);
    }

    void DeferredRenderer::SSAOBlurPass()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, ssao->blurFrameBuffer);
        glClear(GL_COLOR_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssao->aoBuffer);
        ssao->ssaoBlurShader->use();
        ssao->ssaoBlurShader->setTexture("ao", 0);
        quad->Draw();
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

    void DeferredRenderer::CreateTextureMS(unsigned int tex, unsigned int samples, int width, int height, GLint internalFormat, GLenum attachment)
    {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D_MULTISAMPLE, tex, 0);
    }

    void DeferredRenderer::ResolveMS()
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBufferMS);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        glReadBuffer(GL_COLOR_ATTACHMENT1);
        glDrawBuffer(GL_COLOR_ATTACHMENT1);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        glReadBuffer(GL_COLOR_ATTACHMENT2);
        glDrawBuffer(GL_COLOR_ATTACHMENT2);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}