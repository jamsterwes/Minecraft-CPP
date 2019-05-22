#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "../gfx/iv_buffer.hpp"
#include "../gfx/shaders.hpp"
#include "../gfx/textures.hpp"
#include "../lighting/deferred.hpp"

namespace minecraft
{
    enum class BlockType
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

        void CreateInstanceData();
        void LinkToRenderer(lighting::DeferredRenderer& renderer);
        // void Draw(gfx::texture2D& atlas);

        BlockType GetBlockAt(int x, int y, int z);
        void SetBlockAt(BlockType type, int x, int y, int z);

        long GetInstanceCount();
        // gfx::shader& GetShader();

        glm::vec3 chunkOffset;
    private:
        void VertexAttributeProvider();

        // gfx::iv_buffer<> buffer;
        // gfx::shader shader;
        BlockType rawData[16][256][16];
        std::vector<BlockInstanceData> instanceData;
        unsigned int instanceDataBuffer;
    };
}