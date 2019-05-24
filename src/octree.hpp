#pragma once

template<typename DataType>
class Octree
{
public:
    int dim;
    bool divided;
    DataType data;
    class Octree<DataType>* children;

    Octree() : dim(16), divided(false), children(nullptr) { }

    Octree(DataType data) : dim(16), divided(false), data(data), children(nullptr) { }

    void Subdivide()
    {
        if (dim <= 1) return;
        if (divided) return;

        divided = true;
        children = new class Octree<DataType>[8];
        for (int i = 0; i < 8; i++)
        {
            children[i].data = data;
            children[i].dim = dim / 2;
        }
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