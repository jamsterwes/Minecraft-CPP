#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "../gfx/iv_buffer.hpp"
#include "../gfx/shaders.hpp"
#include "../gfx/textures.hpp"
#include "../lighting/deferred.hpp"

#define RAW_DATA_INDEX(x, y, z) x + y * 256 + z * 16

namespace minecraft
{
    enum class BlockType : char
    {
        Air, Grass, Stone, Dirt, Bedrock, Log, Leaves
    };

    struct BlockInstanceData
    {
        glm::vec3 pos;
        glm::vec2 uv;

        static glm::vec2 BlockTypeToUV(BlockType type);
    };

    class Chunk
    {
    public:
        Chunk();
        Chunk(glm::vec3 chunkOffset);

        void CreateInstanceData(std::vector<BlockInstanceData>& worldBin);

        int GetLowestSurface();

        BlockType GetBlockAt(int x, int y, int z);
        void SetBlockAt(BlockType type, int x, int y, int z);

        glm::vec3 chunkOffset;
    private:
        BlockType* rawData;  // REPLACE WITH OCTREE ONE DAY?
    };
}