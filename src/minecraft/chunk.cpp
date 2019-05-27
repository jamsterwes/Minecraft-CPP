#include "chunk.hpp"
#include "../models/cube.hpp"
#include <algorithm>
#include <iostream>
#include <cstdlib>

namespace minecraft
{
    glm::vec2 BlockInstanceData::BlockTypeToUV(BlockType type)
    {
        switch (type)
        {
        case BlockType::Grass:
            return glm::vec2(0, 0);
        case BlockType::Stone:
            return glm::vec2(3, 0);
        case BlockType::Dirt:
            return glm::vec2(0, 1);
        case BlockType::Bedrock:
            return glm::vec2(3, 1);
        case BlockType::Log:
            return glm::vec2(0, 2);
        case BlockType::Leaves:
            return glm::vec2(3, 2);
        default:
            return glm::vec2(-1, -1);
        }
    }

    Chunk::Chunk() : chunkOffset(0.0f, 0.0f, 0.0f) {
        InitOctree();
    }
    Chunk::Chunk(glm::vec3 chunkOffset) : chunkOffset(chunkOffset) {
        InitOctree();
    }

    Chunk::~Chunk()
    {
    }

    void Chunk::Consolidate()
    {
        octree->Consolidate();
    }

    BlockChunkData Chunk::GetBlockAt(int x, int y, int z)
    {
        Octree<BlockChunkData>* ptr = octree;
        int xi = x;
        int yi = y;
        int zi = z;
        int c = CHUNK_SIZE / 2;
        while (c > 0)
        {
            // Step 1: Check division
            if (!ptr->divided) return ptr->data;
            // Step 2: Pick a child
            int octX = (xi >= c) ? 1 : 0; if (xi >= c) xi -= c;
            int octY = (yi >= c) ? 1 : 0; if (yi >= c) yi -= c;
            int octZ = (zi >= c) ? 1 : 0; if (zi >= c) zi -= c;
            // Step 3: Update ptr
            ptr = ptr->GetChild(octX, octY, octZ);
            // Step 4: Update c
            if (c > 1) c /= 2;
            else c = 0;
        }
        return ptr->data;
    }

    void Chunk::SetBlockAt(BlockType type, int x, int y, int z)
    {
        Octree<BlockChunkData>* ptr = octree;
        int xi = x;
        int yi = y;
        int zi = z;
        int c = CHUNK_SIZE / 2;
        while (c > 0)
        {
            // Step 1: Pick a child
            int octX = (xi >= c) ? 1 : 0; if (xi >= c) xi -= c;
            int octY = (yi >= c) ? 1 : 0; if (yi >= c) yi -= c;
            int octZ = (zi >= c) ? 1 : 0; if (zi >= c) zi -= c;
            // Step 2: Subdivide/update ptr
            ptr = ptr->GetChild(octX, octY, octZ);
            // Step 3: Update c
            if (c > 1) c /= 2;
            else c = 0;
        }
        ptr->data.type = type;
    }

    void Chunk::InitOctree()
    {
        octree = new Octree<BlockChunkData>{};
        octree->data.type = BlockType::Air;
        octree->data.outside = 0;
        octree->dim = CHUNK_SIZE;
    }
}