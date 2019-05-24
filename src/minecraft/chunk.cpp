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
        case BlockType::Air:
            return glm::vec2(-1, -1);
        }
    }

    Chunk::Chunk() : chunkOffset(0.0f, 0.0f, 0.0f) {
        InitOctrees();
    }
    Chunk::Chunk(glm::vec3 chunkOffset) : chunkOffset(chunkOffset) {
        InitOctrees();
    }

    Chunk::~Chunk()
    {
    }

    std::string XYZString(glm::vec3 vec)
    {
        return "(" + std::to_string(vec.r) + ", " + std::to_string(vec.g) + ", " + std::to_string(vec.b) + ")";
    }

    void Chunk::OctreeToBin(Octree<BlockType>& tree, glm::vec3 offset, std::vector<BlockInstanceData>& worldBin)
    {
        if (!tree.divided && tree.data == BlockType::Air) return;
        else if (!tree.divided)
        {
            // if (tree.dim == 1) return;
            worldBin.push_back({
                chunkOffset + offset,
                BlockInstanceData::BlockTypeToUV(tree.data),
                static_cast<float>(tree.dim)
            });
        }
        else
        {
            for (int x = 0; x < 2; x++)
            {
                for (int y = 0; y < 2; y++)
                {
                    for (int z = 0; z < 2; z++)
                    {
                        Octree<BlockType>& newTree = *(tree.GetChild(x, y, z));
                        OctreeToBin(newTree, offset + glm::vec3(x, y, z) * glm::vec3(tree.dim / 2), worldBin);
                    }
                }
            }
        }
    }

    void Chunk::CreateInstanceData(std::vector<BlockInstanceData>& worldBin)
    {
        for (int i = 15; i >= 0; i--)
        {
            octrees[i].Consolidate();
            OctreeToBin(octrees[i], glm::vec3(0.0, 16.0 * i, 0.0), worldBin);
        }
    }

    std::string XZString(glm::vec3 vec)
    {
        return "(" + std::to_string(vec.r) + ", " + std::to_string(vec.b) + ")";
    }

    int Chunk::GetLowestSurface()
    {
        int minHeight = 255;
        // for (int x = 0; x < 16; x++)
        // {
        //     for (int z = 0; z < 16; z++)
        //     {
        //         for (int y = 255; y >= 0; y--)
        //         {
        //             if (rawData[RAW_DATA_INDEX(x, y, z)] != BlockType::Air)
        //             {
        //                 if (y < minHeight) minHeight = y;
        //                 break;
        //             }
        //         }
        //     }
        // }
        // std::cout << "min height for chunk " << XZString(chunkOffset) << ": " << minHeight << std::endl;
        return minHeight;
    }

    BlockType Chunk::GetBlockAt(int x, int y, int z)
    {
        int octreeID = floor(y / 16.0);
        Octree<BlockType>* ptr = &octrees[octreeID % 16];
        int xi = x;
        int yi = (y % 16);
        int zi = z;
        int c = 8;
        while (c > 0)
        {
            // Step 1: Check division
            if (!ptr->divided) return ptr->data;
            // Step 2: Pick a child
            int octX = (x >= c) ? 1 : 0;
            int octY = (y >= c) ? 1 : 0;
            int octZ = (z >= c) ? 1 : 0;
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
        int octreeID = floor(y / 16.0);
        Octree<BlockType>* ptr = &octrees[octreeID % 16];
        int xi = x;
        int yi = (y % 16);
        int zi = z;
        int c = 8;
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
        ptr->data = type;
    }

    void Chunk::InitOctrees()
    {
        octrees = new Octree<BlockType>[16];
        for (int i = 0; i < 16; i++)
        {
            octrees[i].data = BlockType::Air;
        }
    }
}