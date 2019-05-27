#pragma once
#include <glad/glad.h>
#include <functional>
#include "util.hpp"
#include <list>
#include <initializer_list>

namespace gfx
{
    template <typename vertexType>
    class v_buffer
    {
    public:
        v_buffer() : indexCount(0)
        {
            glGenVertexArrays(1, &this->vaoID);
            glGenBuffers(1, &this->vboID);
            glGenBuffers(1, &this->eboID);
            internalData = new std::vector<vertexType>{};
            indexData = new std::vector<unsigned int>{};
        }

        const unsigned int indexDataTri[6] = {
            0, 1, 2, 2, 3, 0
        };

        void PushBack(vertexType vertex0, vertexType vertex1, vertexType vertex2, vertexType vertex3)
        {
            unsigned int indexOffset = internalData->size();
            internalData->push_back(vertex0);
            internalData->push_back(vertex1);
            internalData->push_back(vertex2);
            internalData->push_back(vertex3);
            for (int i = 0; i < 6; i++)
            {
                indexData->push_back(indexOffset + indexDataTri[i]);
            }
        }

        void Update(std::function<void()> vertexAttributeProvider)
        {
            glBindVertexArray(this->vaoID);
            glDeleteBuffers(1, &this->vboID);
            glGenBuffers(1, &this->vboID);
            glBindBuffer(GL_ARRAY_BUFFER, this->vboID);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->eboID);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertexType) * internalData->size(), internalData->data(), GL_DYNAMIC_DRAW);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexData->size(), indexData->data(), GL_DYNAMIC_DRAW);
            vertexAttributeProvider();
            glBindVertexArray(0);
            indexCount = indexData->size();
            delete internalData;
            internalData = new std::vector<vertexType>{};
            delete indexData;
            indexData = new std::vector<unsigned int>{};
        }

        void Draw()
        {
            glBindVertexArray(this->vaoID);
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }
        
        unsigned int vaoID;
        unsigned int indexCount;
    private:
        std::vector<vertexType>* internalData;
        std::vector<unsigned int>* indexData;
        unsigned int vboID, eboID;
    };
}