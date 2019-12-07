#pragma once
#include "chunk.hpp"
#include "../gfx/iv_buffer.hpp"
#include "../gfx/v_buffer.hpp"

namespace minecraft
{
    struct BlockVertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec4 uv;

        BlockVertex() : position(0), normal(0), uv(0) { }
        BlockVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv, int atlasIndex, int scale) : position(position), normal(normal), uv(uv.x, uv.y, static_cast<float>(atlasIndex), static_cast<float>(scale)) { }
    };

    struct AtlasIndex
    {
        unsigned int side, top, bottom;
    };

    class MeshRenderer
    {
    public:
        MeshRenderer();
        void AddChunk(minecraft::Chunk chunk, int outsideFlags = 0);
        void Draw();
        void Update();

        AtlasIndex GetAtlasIndex(BlockType type);

        gfx::v_buffer<BlockVertex>* chunkData;
        std::vector<gfx::iv_buffer<>> foliageData;
    private:
        bool CheckPlane(Octree<BlockChunkData>* root, glm::vec3 direction);
        int EdgeOccupancy(Octree<BlockChunkData>* node, glm::vec3 coords);
        void RenderOctree(glm::vec3 offset, Octree<BlockChunkData>* tree, int outsideFlags = 0);

        void LeftFace(glm::vec3 origin, int atlasIndex, int scale);
        void RightFace(glm::vec3 origin, int atlasIndex, int scale);
        void FrontFace(glm::vec3 origin, int atlasIndex, int scale);
        void BackFace(glm::vec3 origin, int atlasIndex, int scale);
        void TopFace(glm::vec3 origin, int atlasIndex, int scale);
        void BottomFace(glm::vec3 origin, int atlasIndex, int scale);
        inline void Face(glm::vec3 p[4], glm::vec3 normal, glm::vec3 origin, int atlasIndex, int scale);
    
        // void AllSelectFaces(glm::vec3 offset, Octree<BlockChunkData>* tree, int outsideFlags);
        // DEBUG
        void DBG_SelectFaces(glm::vec3 offset, int dim, int outsideFlags);
    };
}