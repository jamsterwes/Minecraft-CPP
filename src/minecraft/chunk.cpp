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
        rawData = (BlockType*)calloc(16 * 256 * 16, sizeof(BlockType));
    }
    Chunk::Chunk(glm::vec3 chunkOffset) : chunkOffset(chunkOffset) {
        rawData = (BlockType*)calloc(16 * 256 * 16, sizeof(BlockType));
    }

    void Chunk::CreateInstanceData(std::vector<BlockInstanceData>& worldBin)
    {
        int minHeight = std::max(0, Chunk::GetLowestSurface() - 2);
        for (int x = 0; x < 16; x++)
        {
            for (int y = minHeight; y < 256; y++)
            {
                for (int z = 0; z < 16; z++)
                {
                    if (rawData[RAW_DATA_INDEX(x, y, z)] != BlockType::Air)
                    {
                        worldBin.push_back({
                            glm::vec3(x, y, z) + chunkOffset,
                            BlockInstanceData::BlockTypeToUV(rawData[RAW_DATA_INDEX(x, y, z)])
                        });
                    }
                }
            }
        }
    }

    std::string XZString(glm::vec3 vec)
    {
        return "(" + std::to_string(vec.r) + ", " + std::to_string(vec.b) + ")";
    }

    int Chunk::GetLowestSurface()
    {
        int minHeight = 255;
        for (int x = 0; x < 16; x++)
        {
            for (int z = 0; z < 16; z++)
            {
                for (int y = 255; y >= 0; y--)
                {
                    if (rawData[RAW_DATA_INDEX(x, y, z)] != BlockType::Air)
                    {
                        if (y < minHeight) minHeight = y;
                        break;
                    }
                }
            }
        }
        std::cout << "min height for chunk " << XZString(chunkOffset) << ": " << minHeight << std::endl;
        return minHeight;
    }

    BlockType Chunk::GetBlockAt(int x, int y, int z)
    {
        return rawData[RAW_DATA_INDEX(x, y, z)];
    }

    void Chunk::SetBlockAt(BlockType type, int x, int y, int z)
    {
        rawData[RAW_DATA_INDEX(x, y, z)] = type;
    }
}