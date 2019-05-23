#pragma once
#include <glad/glad.h>
#include <functional>
#include "util.hpp"

namespace gfx
{
    template <typename indexType = unsigned int, typename vertexType = float>
    class iv_buffer
    {
    public:
        iv_buffer() : indexCount(0)
        {
            glGenVertexArrays(1, &this->vaoID);
            glGenBuffers(1, &this->vboID);
            glGenBuffers(1, &this->eboID);
        }

        iv_buffer(const indexType* indices, size_t indexCount, const vertexType* vertices, size_t vertexCount, std::function<void()> vertexAttributeProvider) : indexCount(indexCount)
        {
            glGenVertexArrays(1, &this->vaoID);
            glGenBuffers(1, &this->vboID);
            glGenBuffers(1, &this->eboID);

            glBindVertexArray(this->vaoID);

            glBindBuffer(GL_ARRAY_BUFFER, this->vboID);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertexType) * vertexCount, vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->eboID);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexType) * indexCount, indices, GL_STATIC_DRAW);

            vertexAttributeProvider();

            glBindVertexArray(0);
        }

        void Draw(GLenum glIndexType = GL_UNSIGNED_INT)
        {
            glBindVertexArray(this->vaoID);
            glDrawElements(GL_TRIANGLES, this->indexCount, glIndexType, 0);
            glBindVertexArray(0);
        }
    
        void DrawInstanced(GLenum glIndexType, int instanceCount)
        {
            glBindVertexArray(this->vaoID);
            glDrawElementsInstanced(GL_TRIANGLES, this->indexCount, glIndexType, 0, instanceCount);
            glBindVertexArray(0);
        }
        
        unsigned int vaoID;
    private:
        unsigned int vboID, eboID;
        size_t indexCount;
    };
}