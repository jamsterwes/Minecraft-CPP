#include "world_renderer.hpp"

namespace minecraft
{
    WorldRenderer::WorldRenderer() : chunks() { }

    void WorldRenderer::RegisterChunk(Chunk chunk)
    {
        chunks.push_back(chunk);
    }

    void WorldRenderer::CreateInstanceData()
    {
        instanceData = new std::vector<BlockInstanceData>();
        for (int i = 0; i < chunks.size(); i++)
        {
            chunks[i].CreateInstanceData(*instanceData);
        }

        glGenBuffers(1, &instanceDataBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, instanceDataBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(BlockInstanceData) * instanceData->size(), instanceData->data(), GL_STATIC_DRAW);
    }

    void WorldRenderer::LinkToRenderer(lighting::DeferredRenderer& renderer)
    {
        glBindVertexArray(renderer.GetVAO());
        glBindBuffer(GL_ARRAY_BUFFER, instanceDataBuffer);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
        glEnableVertexAttribArray(6);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(6, 1);
        glBindVertexArray(0);
    }

    void WorldRenderer::ClearChunks()
    {
        chunks = std::vector<Chunk>{};
    }

    long WorldRenderer::GetChunkCount()
    {
        return chunks.size();
    }
    
    long WorldRenderer::GetInstanceCount()
    {
        return instanceData->size();
    }
}