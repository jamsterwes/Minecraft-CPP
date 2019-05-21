#pragma once
#include <glm/glm.hpp>

namespace minecraft
{
    namespace BlockUV
    {
        glm::vec2 GRASS = glm::vec2(0, 0);
        glm::vec2 STONE = glm::vec2(3, 0);
        glm::vec2 DIRT = glm::vec2(0, 1);
    }

    struct BlockInstanceData
    {
        glm::vec3 pos;
        glm::vec2 uv;
    };

    class Chunk
    {

    };
}