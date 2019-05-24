#pragma once
#include <glm/glm.hpp>
#include "../gfx/shaders.hpp"

namespace lighting
{
    class ShadowCam
    {
    public:
        ShadowCam();
        glm::mat4 GetProj();
        glm::mat4 GetView(glm::vec3 LightDir);
        glm::mat4 GetLightSpace(glm::vec3 LightDir);
        unsigned int shadowFramebuffer, shadowMap;
        int shadowMapResolution;

        gfx::shader* shadowShader;
    private:
        void GenerateBuffers();

        glm::mat4 proj;
    };
}