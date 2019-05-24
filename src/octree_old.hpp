#pragma once
#include <array>
#include <cstdlib>
#include <cstring>

template<typename DataType>
class Octree;

template<typename DataType>
class Octree
{
public:
    DataType data;
    int dim;
	bool divided; // if false, no children;
    std::array<class Octree<DataType>, 8> children;
    
    Octree() : dim(1), divided(false), children() {}

    Octree(DataType& data) : divided(false), children()
    {
        children = nullptr;
        this->SetData(data);
    }

    ~Octree()
    {
        this->Undivide();
        this->Free();
    }

    void Subdivide()
    {
        divided = true;
		for (int i = 0; i < 8; i++)
		{
            children[i]->SetData(this->data);
			children[i]->dim = dim - 1;
		}
    }

    void Undivide()
    {
        divided = false;
        children = std::array<Octree<DataType>, 8>();
    }

    Octree<DataType>& GetChild(bool x, bool y, bool z)
    {
        return children[x + y * 2 + z * 4];
    }

    void SetData(DataType& data)
    {
        this->data = data;
    }
};