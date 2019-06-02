#include "mesh_renderer.hpp"
#include <iostream>

namespace minecraft
{
    MeshRenderer::MeshRenderer()
    {
        chunkData = new gfx::v_buffer<BlockVertex>();
    }

    void MeshRenderer::AddChunk(minecraft::Chunk chunk, int outsideFlags)
    {
        RenderOctree(glm::vec3(0.0, 0.0, 0.0) + chunk.chunkOffset, chunk.octree, outsideFlags);
    }

    void MeshRenderer::Draw()
    {
        chunkData->Draw();
    }

    void MeshRenderer::Update()
    {
        chunkData->Update([]() {
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));
            glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
        });
    }

    AtlasIndex MeshRenderer::GetAtlasIndex(BlockType type)
    {
        switch(type)
        {
        case BlockType::Grass:
            return {0, 1, 2};
        case BlockType::Dirt:
            return {2, 2, 2};
        case BlockType::Stone:
            return {3, 3, 3};
        case BlockType::Bedrock:
            return {4, 4, 4};
        case BlockType::Log:
            return {5, 6, 6};
        case BlockType::Leaves:
            return {7, 7, 7};
        case BlockType::Sand:
            return {8, 8, 8};
        default:
            return {0, 0, 0};
        }
    }

    typedef Octree<BlockChunkData> edge;
    bool MeshRenderer::CheckPlane(Octree<BlockChunkData>* root, glm::vec3 direction)
    {
        if (!root->divided) return root->data.type == BlockType::Air;
        for (int x = 0; x < 2; x++)
        {
            for (int y = 0; y < 2; y++)
            {
                for (int z = 0; z < 2; z++)
                {
                    if (glm::vec3(2 * x - 1, 2 * y - 1, 2 * z - 1) * direction != direction * direction) continue;
                    edge* node = root->GetChild(x, y, z);
                    bool nodeCheck = false;
                    if (node->divided) nodeCheck = CheckPlane(node, direction);
                    else nodeCheck = node->data.type == BlockType::Air;
                    if (nodeCheck)
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    edge* FindPath(edge* node, glm::vec3 path, int res)
    {
        edge* parent = node;
        while (parent->parent != nullptr) parent = parent->parent;

        edge* ptr = parent;
        int xi = path.x;
        int yi = path.y;
        int zi = path.z;
        int c = CHUNK_SIZE / 2;
        while (c > res / 2)
        {
            if (!ptr->divided) return ptr;
            int octX = (xi >= c) ? 1 : 0; if (xi >= c) xi -= c;
            int octY = (yi >= c) ? 1 : 0; if (yi >= c) yi -= c;
            int octZ = (zi >= c) ? 1 : 0; if (zi >= c) zi -= c;
            ptr = ptr->GetChild(octX, octY, octZ);
            if (c > 1) c /= 2;
            else c = 0;
        }
        return ptr;
    }

    int MeshRenderer::EdgeOccupancy(Octree<BlockChunkData>* node, glm::vec3 coords)
    {
        int cull = 63;

        // CULL COUSINS
        glm::vec3 path = node->GetPath();

        for (int d = 0; d < 2; d++)
        {
            int direction = (2 * d) - 1;
            float actualD = path.x + direction * node->dim;
            if (actualD < 0 && direction == -1 || actualD > CHUNK_SIZE - 1 && direction == 1) continue;
            glm::vec3 actualPath = glm::vec3(actualD, path.y, path.z);
            // FIND PATH
            edge* foundPath = FindPath(node, actualPath, node->dim);
            if (!CheckPlane(foundPath, glm::vec3(direction, 0.0, 0.0))) cull ^= (int)(direction == -1 ? OutsideFlags::NX : OutsideFlags::PX);
        }
        for (int d = 0; d < 2; d++)
        {
            int direction = (2 * d) - 1;
            float actualD = path.y + direction * node->dim;
            if (actualD < 0 && direction == -1 || actualD > CHUNK_SIZE - 1 && direction == 1) continue;
            glm::vec3 actualPath = glm::vec3(path.x, actualD, path.z);
            // FIND PATH
            edge* foundPath = FindPath(node, actualPath, node->dim);
            if (!CheckPlane(foundPath, glm::vec3(0.0, direction, 0.0))) cull ^= (int)(direction == -1 ? OutsideFlags::NY : OutsideFlags::PY);
        }
        for (int d = 0; d < 2; d++)
        {
            int direction = (2 * d) - 1;
            float actualD = path.z + direction * node->dim;
            if (actualD < 0 && direction == -1 || actualD > CHUNK_SIZE - 1 && direction == 1) continue;
            glm::vec3 actualPath = glm::vec3(path.x, path.y, actualD);
            // FIND PATH
            edge* foundPath = FindPath(node, actualPath, node->dim);
            if (!CheckPlane(foundPath, glm::vec3(0.0, 0.0, direction))) cull ^= (int)(direction == -1 ? OutsideFlags::NZ : OutsideFlags::PZ);
        }

        return cull;
    }

    void MeshRenderer::RenderOctree(glm::vec3 offset, Octree<BlockChunkData>* tree, int outsideFlags)
    {
        Octree<BlockChunkData>* ptr = tree;
        if (!ptr->divided && ptr->data.type == BlockType::Air) return;
        else if (!ptr->divided)
        {
            if ((outsideFlags & (int)OutsideFlags::NX) == (int)OutsideFlags::NX) LeftFace(offset, GetAtlasIndex(ptr->data.type).side, ptr->dim);
            if ((outsideFlags & (int)OutsideFlags::PX) == (int)OutsideFlags::PX) RightFace(offset, GetAtlasIndex(ptr->data.type).side, ptr->dim);
            if ((outsideFlags & (int)OutsideFlags::PZ) == (int)OutsideFlags::PZ) FrontFace(offset, GetAtlasIndex(ptr->data.type).side, ptr->dim);
            if ((outsideFlags & (int)OutsideFlags::NZ) == (int)OutsideFlags::NZ) BackFace(offset, GetAtlasIndex(ptr->data.type).side, ptr->dim);
            if ((outsideFlags & (int)OutsideFlags::PY) == (int)OutsideFlags::PY) TopFace(offset, GetAtlasIndex(ptr->data.type).top, ptr->dim);
            if ((outsideFlags & (int)OutsideFlags::NY) == (int)OutsideFlags::NY) BottomFace(offset, GetAtlasIndex(ptr->data.type).bottom, ptr->dim);
        }
        else
        {
            for (int x = 0; x < 2; x++)
            {
                for (int y = 0; y < 2; y++)
                {
                    for (int z = 0; z < 2; z++)
                    {
                        Octree<BlockChunkData>* newTree = ptr->GetChild(x, y, z);
                        int outFlags = (newTree->divided ? 63 : EdgeOccupancy(newTree, glm::vec3(x, y, z)));
                        RenderOctree(offset + glm::vec3(x, y, z) * glm::vec3(ptr->dim / 2), newTree, outFlags);
                    }
                }
            }
        }
    }

    const glm::vec3 LeftFaceData[4] = {
        glm::vec3(0.0, 0.0, 0.0),
        glm::vec3(0.0, 0.0, 1.0),
        glm::vec3(0.0, 1.0, 1.0),
        glm::vec3(0.0, 1.0, 0.0)
    };
    void MeshRenderer::LeftFace(glm::vec3 origin, int atlasIndex, int scale)
    {
        Face(LeftFaceData, glm::vec3(-1, 0, 0), origin, atlasIndex, scale);
    }

    const glm::vec3 RightFaceData[4] = {
        glm::vec3(1.0, 0.0, 1.0),
        glm::vec3(1.0, 0.0, 0.0),
        glm::vec3(1.0, 1.0, 0.0),
        glm::vec3(1.0, 1.0, 1.0)
    };
    void MeshRenderer::RightFace(glm::vec3 origin, int atlasIndex, int scale)
    {
        Face(RightFaceData, glm::vec3(1, 0, 0), origin, atlasIndex, scale);
    }

    const glm::vec3 FrontFaceData[4] = {
        glm::vec3(0.0, 0.0, 1.0),
        glm::vec3(1.0, 0.0, 1.0),
        glm::vec3(1.0, 1.0, 1.0),
        glm::vec3(0.0, 1.0, 1.0)
    };
    void MeshRenderer::FrontFace(glm::vec3 origin, int atlasIndex, int scale)
    {
        Face(FrontFaceData, glm::vec3(0, 0, 1), origin, atlasIndex, scale);
    }

    const glm::vec3 BackFaceData[4] = {
        glm::vec3(1.0, 0.0, 0.0),
        glm::vec3(0.0, 0.0, 0.0),
        glm::vec3(0.0, 1.0, 0.0),
        glm::vec3(1.0, 1.0, 0.0)
    };
    void MeshRenderer::BackFace(glm::vec3 origin, int atlasIndex, int scale)
    {
        Face(BackFaceData, glm::vec3(0, 0, -1), origin, atlasIndex, scale);
    }

    const glm::vec3 TopFaceData[4] = {
        glm::vec3(0.0, 1.0, 1.0),
        glm::vec3(1.0, 1.0, 1.0),
        glm::vec3(1.0, 1.0, 0.0),
        glm::vec3(0.0, 1.0, 0.0)
    };
    void MeshRenderer::TopFace(glm::vec3 origin, int atlasIndex, int scale)
    {
        Face(TopFaceData, glm::vec3(0, 1, 0), origin, atlasIndex, scale);
    }

    const glm::vec3 BottomFaceData[4] = {
        glm::vec3(0.0, 0.0, 0.0),
        glm::vec3(1.0, 0.0, 0.0),
        glm::vec3(1.0, 0.0, 1.0),
        glm::vec3(0.0, 0.0, 1.0)
    };
    void MeshRenderer::BottomFace(glm::vec3 origin, int atlasIndex, int scale)
    {
        Face(BottomFaceData, glm::vec3(0, -1, 0), origin, atlasIndex, scale);
    }

    void MeshRenderer::Face(const glm::vec3 p[4], glm::vec3 normal, glm::vec3 origin, int atlasIndex, int scale)
    {
        chunkData->PushBack(BlockVertex(
            p[0] * glm::vec3(scale) + origin,
            normal,
            glm::vec2(1.0, 1.0), atlasIndex, scale
        ), BlockVertex(
            p[1] * glm::vec3(scale) + origin,
            normal,
            glm::vec2(0.0, 1.0), atlasIndex, scale
        ), BlockVertex(
            p[2] * glm::vec3(scale) + origin,
            normal,
            glm::vec2(0.0, 0.0), atlasIndex, scale
        ), BlockVertex(
            p[3] * glm::vec3(scale) + origin,
            normal,
            glm::vec2(1.0, 0.0), atlasIndex, scale
        ));
    }
}