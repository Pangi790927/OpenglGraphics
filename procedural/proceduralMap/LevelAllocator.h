#ifndef LEVEL_ALLOCATOR_H
#define LEVEL_ALLOCATOR_H

#include "AllocOnVec.h"

template <typename MapVertType>
class LevelAllocator {
public:
	AllocOnVec<MapVertexType> allocVerts;
	AllocOnVec<std::vector<int>> allocElems;

	LevelAllocator () {}

	LevelAllocator (int start, int size,
			std::vector<MapVertType> &verts, std::vector<std::vector<int>> &elems) 
			: allocVerts(start, size, verts), allocElems(start, size, elems)
	{}
};

#endif