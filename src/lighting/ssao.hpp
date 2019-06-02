#pragma once
#include <random>
#include <vector>
#include <glm/glm.hpp>
#include "../gfx/shaders.hpp"

namespace lighting
{
    // TODO: initialize ssao shader
    // TODO: implement object oriented post processing effects chain...
    class SSAO
    {
    public:
        SSAO(int width = 800, int height = 600);

        void Resize(int width, int height);

        unsigned int aoBuffer, frameBuffer, noiseTex;
        gfx::shader* ssaoShader;
        unsigned int aoBlurBuffer, blurFrameBuffer;
        gfx::shader* ssaoBlurShader;
        std::vector<glm::vec3> ssaoKernel;
    private:
        void GenerateBuffers(int width, int height);
        void GenerateKernel();
        void GenerateNoise();

        // rendering
        std::vector<glm::vec3> ssaoNoise;

        // noise generation
        std::uniform_real_distribution<float> randomFloats;
        std::default_random_engine generator;
    };
}