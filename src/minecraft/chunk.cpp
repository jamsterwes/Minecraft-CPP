#include "chunk.hpp"
#include "../models/cube.hpp"

namespace minecraft
{
    glm::vec2 BlockInstanceData::BlockTypeToUV(BlockType type)
    {
        switch (type)
        {
        case BlockType::Dirt:
            return glm::vec2(0, 1);
        case BlockType::Grass:
            return glm::vec2(0, 0);
        case BlockType::Stone:
            return glm::vec2(3, 0);
        case BlockType::Air:
            return glm::vec2(-1, -1);
        }
    }

    Chunk::Chunk() : chunkOffset(0.0f, 0.0f, 0.0f), instanceData(), shader("shaders/cube.vs", "shaders/cube.fs")
    {
        buffer = gfx::iv_buffer<>(cube_data::indices, ARRAYSIZE(cube_data::indices), cube_data::vertices, ARRAYSIZE(cube_data::vertices), [] () {
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
        });
        shader.compile();
    }
    Chunk::Chunk(glm::vec3 chunkOffset) : chunkOffset(chunkOffset), instanceData(), shader("shaders/cube.vs", "shaders/cube.fs")
    {
        buffer = gfx::iv_buffer<>(cube_data::indices, ARRAYSIZE(cube_data::indices), cube_data::vertices, ARRAYSIZE(cube_data::vertices), [] () {
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
        });
        shader.compile();
    }

    void Chunk::CreateInstanceData()
    {
        instanceData = std::vector<BlockInstanceData>();
        for (int x = 0; x < 16; x++)
        {
            for (int y = 0; y < 256; y++)
            {
                for (int z = 0; z < 16; z++)
                {
                    if (rawData[x][y][z] != BlockType::Air)
                    {
                        instanceData.push_back({
                            glm::vec3(x, y, z),
                            BlockInstanceData::BlockTypeToUV(rawData[x][y][z])
                        });
                    }
                }
            }
        }

        glBindVertexArray(buffer.vaoID);
        glGenBuffers(1, &instanceDataBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, instanceDataBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(BlockInstanceData) * instanceData.size(), &instanceData[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glBindVertexArray(0);
    }

    void Chunk::Draw(gfx::texture2D& atlas)
    {
        shader.use();

        atlas.slot(GL_TEXTURE0);
        shader.setTexture("Atlas", 0);

        shader.setVector("chunkOffset", chunkOffset);
        shader.setMatrix("model", glm::mat4(1.0f));

        buffer.Draw(GL_UNSIGNED_INT, instanceData.size());
    }

    BlockType Chunk::GetBlockAt(int x, int y, int z)
    {
        return rawData[x][y][z];
    }

    void Chunk::SetBlockAt(BlockType type, int x, int y, int z)
    {
        rawData[x][y][z] = type;
    }

    long Chunk::GetInstanceCount()
    {
        return instanceData.size();
    }

    gfx::shader& Chunk::GetShader()
    {
        return shader;
    }
}