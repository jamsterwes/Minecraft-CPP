#include "deferred.hpp"

namespace lighting
{
    DeferredRenderer::DeferredRenderer(int width, int height)
    {
        glGenFramebuffers(1, &gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glGenTextures(1, &gPosition);
        glGenTextures(1, &gNormal);
        glGenTextures(1, &gAlbedoSpec);
        Resize(width, height);

        unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void DeferredRenderer::Resize(int width, int height)
    {
        CreateTexture(gPosition, width, height, GL_RGB16F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0);
        CreateTexture(gNormal, width, height, GL_RGB16F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT1);
        CreateTexture(gAlbedoSpec, width, height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT2);
    }

    void DeferredRenderer::Use(gfx::shader& shaderLighting)
    {
        // glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glBindTexture(GL_TEXTURE_2D, gPosition);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, gNormal);
        // glActiveTexture(GL_TEXTURE2);
        // glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

        // shaderLighting.use();
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