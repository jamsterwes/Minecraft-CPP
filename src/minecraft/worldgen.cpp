#include "worldgen.hpp"
#include "simplex1234.hpp"

namespace minecraft
{
    float min(float a, float b)
    {
        return (a > b) ? b : a;
    }

    float max(float a, float b)
    {
        return (a > b) ? a : b;
    }

    float WorldGen::FBM(glm::vec2 coords, float freq, float lac, int oct, float bias, float upperScale, float lowerScale)
    {
        float t = 0.0f;
        float amp = 1.0f;
        float ampSum = 0.0f;
        for (int k = 0; k < oct; k++)
        {
            glm::vec2 C = coords * freq;
            t += min(SimplexNoise1234::noise(C.x, C.y), 1.0) * amp;
            ampSum += amp;
            amp *= 0.5;
            freq = lac;
        }
        float out = t / ampSum;
        out = ((out > bias) ? upperScale : lowerScale) * (out - bias) + bias;
        return out;
    }

    float WorldGen::FBM(glm::vec3 coords, float freq, float lac, int oct, float bias, float upperScale, float lowerScale)
    {
        float t = 0.0f;
        float amp = 1.0f;
        float ampSum = 0.0f;
        for (int k = 0; k < oct; k++)
        {
            glm::vec3 C = coords * freq;
            t += min(SimplexNoise1234::noise(C.x, C.y, C.z), 1.0) * amp;
            ampSum += amp;
            amp *= 0.5;
            freq = lac;
        }
        float out = t / ampSum;
        out = ((out > bias) ? upperScale : lowerScale) * (out - bias) + bias;
        return out;
    }
}