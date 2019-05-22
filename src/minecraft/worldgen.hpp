#pragma once
#include <glm/glm.hpp>

namespace minecraft
{
    class WorldGen
    {
    public:
        static float FBM(glm::vec2 coords, float freq, float lac, int oct, float bias, float upperScale, float lowerScale);
        static float FBM(glm::vec3 coords, float freq, float lac, int oct, float bias, float upperScale, float lowerScale);
    };
}