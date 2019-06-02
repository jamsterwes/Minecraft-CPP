#include "deferred.hpp"
#include "../models/cube.hpp"
#include "../minecraft/world_renderer.hpp"
#include <glm/gtx/transform.hpp>

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
        glGenRenderbuffers(1, &gDepthStencil);
        Resize(width, height);
        // Attach G-Buffer sub-buffers
        unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
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

        CreateTexture(gPosition, width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0);
        CreateTexture(gNormal, width, height, GL_RGB16F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT1);
        CreateTexture(gAlbedoSpec, width, height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT2);
    
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, gDepthStencil);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gDepthStencil);
    }
    
    void DeferredRenderer::ShadowMapPass(minecraft::WorldRenderer& world, ShadowCam& cam)
    {
        glViewport(0, 0, cam.shadowMapResolution, cam.shadowMapResolution);
        glBindFramebuffer(GL_FRAMEBUFFER, cam.shadowFramebuffer);

        cam.shadowShader->use();
        cam.shadowShader->setMatrix("lightSpaceMat", cam.GetLightSpace(glm::vec3(64.0f, -110.0f, 64.0f)));

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);
    }

    void DeferredRenderer::GBufferPass(std::function<void()> drawGBuffer)
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

        drawGBuffer();
        // world.LinkToRenderer(*this);
        // cubeModel->DrawInstanced(GL_UNSIGNED_INT, world.GetInstanceCount());
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
}