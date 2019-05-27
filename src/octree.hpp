#pragma once
#include <glm/glm.hpp>

template<typename DataType>
class Octree
{
public:
    int dim;
    int ID;
    bool divided;
    DataType data;
    class Octree<DataType>* parent;
    class Octree<DataType>* children;

    Octree() : dim(16), divided(false), children(nullptr), parent(nullptr), ID(0) { }

    Octree(DataType data) : dim(16), divided(false), data(data), children(nullptr), parent(nullptr), ID(0) { }

    void Subdivide()
    {
        if (dim <= 1) return;
        if (divided) return;

        divided = true;
        children = new class Octree<DataType>[8];
        for (int i = 0; i < 8; i++)
        {
            children[i].ID = i;
            children[i].parent = this;
            children[i].data = data;
            children[i].dim = dim / 2;
        }
    }

    glm::vec3 GetCoords()
    {
        return glm::vec3(static_cast<float>(ID & 1), static_cast<float>(ID & 2) / 2.0, static_cast<float>(ID & 4) / 4.0);
    }

    glm::vec3 GetPath()
    {
        glm::vec3 path = glm::vec3(0.0);
        class Octree<DataType>* ptr = this;
        while (ptr->parent != nullptr)
        {
            path += ptr->GetCoords() * glm::vec3(ptr->dim);
            ptr = ptr->parent;
        }
        return path;
    }

    void Consolidate()
    {
        if (!divided) return;
        bool canConsolidate = true;
        bool setT = false;
        DataType t;
        for (int i = 0; i < 8; i++)
        {
            children[i].Consolidate();
            if (children[i].divided)
            {
                canConsolidate = false;
                break;
            }
            if (!setT) 
            {
                t = children[i].data;
                setT = true;
                continue;
            }
            if (t != children[i].data)
            {
                canConsolidate = false;
                break;
            }
        }
        if (canConsolidate)
        {
            data = t;
            divided = false;
        }
    }

    bool Contains(DataType t)
    {
        if (!divided) return data == t;
        for (int i = 0; i < 8; i++)
        {
            if (children[i].divided ? children[i].Contains(t) : (children[i].data == t)) return true;
        }
        return false;
    }

    class Octree<DataType>* GetChild(int x, int y, int z)
    {
        this->Subdivide();
        return &children[(x % 2) + (y % 2) * 2 + (z % 2) * 4];
    }
};