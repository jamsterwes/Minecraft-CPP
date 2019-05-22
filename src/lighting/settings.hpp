#pragma once
#include <glm/glm.hpp>
#include "../gfx/colors.hpp"

namespace lighting
{
    struct LightingSettings
    {
        // Light #1 Properties
        glm::vec3 LightDir;
        gfx::color LightColor;

        // Fog
        float FogDensity;
        gfx::color FogColor;

        LightingSettings() : LightColor("#000000"), FogColor("#000000") {};
    };
}