#include "chunk.hpp"
#include "../lighting/deferred.hpp"

namespace minecraft
{
    class WorldRenderer
    {
    public:
        WorldRenderer();
        void RegisterChunk(Chunk chunk);
        void CreateInstanceData();
        void CreateInstanceData_CheckFlags(int flags);
        void LinkToRenderer(lighting::DeferredRenderer& renderer);
        void ClearChunks();
        long GetChunkCount();
        long GetInstanceCount();
    private:
        std::vector<Chunk> chunks;
        std::vector<BlockInstanceData>* instanceData;
        unsigned int instanceDataBuffer;
    };
}