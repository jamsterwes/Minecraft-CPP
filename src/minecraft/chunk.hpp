#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "../gfx/iv_buffer.hpp"
#include "../gfx/shaders.hpp"
#include "../gfx/textures.hpp"
#include "../lighting/deferred.hpp"
#include "../octree.hpp"

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
        float dimension = 1;

        static glm::vec2 BlockTypeToUV(BlockType type);
    };

    class Chunk
    {
    public:
        Chunk();
        Chunk(glm::vec3 chunkOffset);
        ~Chunk();

        void CreateInstanceData(std::vector<BlockInstanceData>& worldBin);

        int GetLowestSurface();

        BlockType GetBlockAt(int x, int y, int z);
        void SetBlockAt(BlockType type, int x, int y, int z);
        // TODO:
        void FillRegion(BlockType type, glm::vec3 start, glm::vec3 end);

        glm::vec3 chunkOffset;
    private:
        void InitOctrees();
        void OctreeToBin(Octree<BlockType>& tree, glm::vec3 offset, std::vector<BlockInstanceData>& worldBin);

        Octree<BlockType>* octrees;
    };
}