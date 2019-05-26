#include "ssao.hpp"
#include <glad/glad.h>
#include <iostream>

namespace lighting
{
    SSAO::SSAO(int width, int height) : randomFloats(0.0, 1.0), generator()
    {
        glGenTextures(1, &noiseTex);
        glGenTextures(1, &aoBuffer);
        glGenFramebuffers(1, &frameBuffer);

        GenerateBuffers(width, height);
        GenerateKernel();
        GenerateNoise();

        ssaoShader = new gfx::shader("shaders/postPass.vert", "shaders/ssao.frag");
        ssaoShader->compile();
    }

    float lerp(float a, float b, float t)
    {
        return a + t * (b - a);
    }

    void SSAO::Resize(int width, int height) { GenerateBuffers(width, height); }

    // TODO: hook into Window::AddResizeAction
    void SSAO::GenerateBuffers(int width, int height)
    {
        // SSAO FRAMEBUFFER
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        glBindTexture(GL_TEXTURE_2D, aoBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, (int)(width / 2), (int)(height / 2), 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, aoBuffer, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSAO Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void SSAO::GenerateKernel()
    {
        ssaoKernel = std::vector<glm::vec3>();
        for (int i = 0; i < 64; i++)
        {
            glm::vec3 sample{randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator)};
            sample = glm::normalize(sample);
            sample *= randomFloats(generator);
            float scale = float(i) / 64.0;

            scale = lerp(0.1f, 1.0f, scale * scale);
            sample *= scale;

            ssaoKernel.push_back(sample);
        }
    }

    void SSAO::GenerateNoise()
    {
        ssaoNoise = std::vector<glm::vec3>();
        for (int i = 0; i < 16; i++)
        {
            glm::vec3 noise{randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f};
            ssaoNoise.push_back(noise);
        }
        glBindTexture(GL_TEXTURE_2D, noiseTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, ssaoNoise.data());  
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}