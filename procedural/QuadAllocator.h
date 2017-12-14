#ifndef QUAD_ALLOCATOR_H
#define QUAD_ALLOCATOR_H

#include "LevelAllocator.h"
#include "DynamicVBOMeshDraw.h"

/// This Class will also have the role of drawing the geometry
/// It should update only the respective part of the geometry wich has 
/// been changed 
class QuadAllocator {
public:
	/// if the quads are thightly packed there is one vert per quad if quads -> inf
	/// in reality we must also acount for the verts on the boundry
	/// so for 3x3 (standard quad) we have a ratio of (3*3 + 3 + 3 + 1)/9 = 16/9 verts/quads
	/// for 9x9 the ratio is (9*9 + 9 + 9 + 1)/81 = 100/81 verts/quads 
	/// (n*n + n + n + 1) = (n * (n + 1) + n + 1) = (n + 1) * (n + 1)
	/// to take into acount this discrepance calcAllocSize() will be used

	/// THIS FORMULA DOES NOT WORK !!!
	static int calcAllocSize (int size) {
		return (size + 1) * (size + 1) * 2 * 2 * 2;	
		/// we will think later (one * 2 is because we need double size
		/// to also keep the old quads/verts)
	}

	static MapVertexType emptyVertex;

	/// the Quad Allocator will also be in charge of the renderer, it will update only
	/// the levels that were modified in the allocator
	Mesh<MapVertexType> mesh;
	DynamicVBOMeshDraw glMesh;

	/// there is an allocator for each level
	std::vector<LevelAllocator<MapVertexType>> allocators;
	
	int size;
	int levels;
	int stackLast = 0;

	std::vector<bool> isElemUpdated;
	std::vector<std::pair<int, int>> elemUpdated;

	std::vector<bool> isVertUpdated;
	std::vector<std::pair<int, int>> vertUpdated;

	QuadAllocator (int size, int levels) 
			: size(size), levels(levels), allocators(levels)
	{
		mesh.vertexList = std::vector<MapVertexType>(size * levels, MapVertexType());
		mesh.elementIndex = std::vector<std::vector<int>>(size * levels, std::vector<int>{0, 0, 0, 0});

		for (int i = 0; i < levels; i++) {
			allocators[i] = LevelAllocator<MapVertexType>(i * size, size, 
					mesh.vertexList, mesh.elementIndex);
		}

		isElemUpdated = std::vector<bool>(levels, false);
		isVertUpdated = std::vector<bool>(levels, false);

		elemUpdated = std::vector<std::pair<int, int>>(levels, std::pair<int, int>(levels * size, 0));
		vertUpdated = std::vector<std::pair<int, int>>(levels, std::pair<int, int>(levels * size, 0));
	}

	int allocVert (int level) {
		int retID = allocators[level].allocVerts.addElem();

		isVertUpdated[level] = true;
		vertUpdated[level].first = std::min(vertUpdated[level].first, retID);
		vertUpdated[level].second = std::max(vertUpdated[level].second, retID);

		return retID;
	}

	void freeVert (int level, int id) {
		isVertUpdated[level] = true;
		
		vertUpdated[level].first = std::min(vertUpdated[level].first, id);
		vertUpdated[level].second = std::max(vertUpdated[level].second, id);

		allocators[level].allocVerts.popFrom(id);
	}

	auto& getVert (int level, int id) {
		isVertUpdated[level] = true;
		
		vertUpdated[level].first = std::min(vertUpdated[level].first, id);
		vertUpdated[level].second = std::max(vertUpdated[level].second, id);

		return allocators[level].allocVerts.get(id);
	}

	const auto& getConstVert (int level, int id) {
		return allocators[level].allocVerts.get(id);
	}

	int allocElem (int level) {
		int retID = allocators[level].allocElems.addElem(); 

		isElemUpdated[level] = true;
		elemUpdated[level].first = std::min(elemUpdated[level].first, retID);
		elemUpdated[level].second = std::max(elemUpdated[level].second, retID);

		return retID;
	}

	void freeElem (int level, int id) {
		isElemUpdated[level] = true;

		elemUpdated[level].first = std::min(elemUpdated[level].first, id);
		elemUpdated[level].second = std::max(elemUpdated[level].second, id);

		allocators[level].allocElems.popFrom(id);
	}

	auto& getElem (int level, int id) {
		isElemUpdated[level] = true;

		elemUpdated[level].first = std::min(elemUpdated[level].first, id);
		elemUpdated[level].second = std::max(elemUpdated[level].second, id);

		return allocators[level].allocElems.get(id);
	}

	const auto& getConstElem (int level, int id) {
		return allocators[level].allocElems.get(id);
	}

	void initDraw() {
		glMesh.init(mesh);
	}

	void updateDrawing() {
		/// must repair the update drawing for maximum efficiency 
		for (int level = 0; level < isElemUpdated.size(); level++) {
			if (isElemUpdated[level]) {
				glMesh.updateElem(
					mesh, 
					elemUpdated[level].first, 
					elemUpdated[level].second - elemUpdated[level].first + 1, 
					DynamicVBOMeshDraw::QUAD
				);
				// std::cout << "level: " << level << " elem: " << elemUpdated[level].first 
				// 		<< ", " << elemUpdated[level].second << std::endl;

				elemUpdated[level] = std::pair<int, int>(levels * size, 0);
				isElemUpdated[level] = false;
			}
		}

		for (int level = 0; level < isVertUpdated.size(); level++) {
			if (isVertUpdated[level]) {
				glMesh.updateVert(
					mesh, 
					vertUpdated[level].first, 
					vertUpdated[level].second - vertUpdated[level].first + 1
				);
				// std::cout << "level: " << level << " vert: " << vertUpdated[level].first 
				// 		<< ", " << vertUpdated[level].second << std::endl;
						
				isVertUpdated[level] = false;
				vertUpdated[level] = std::pair<int, int>(levels * size, 0);
			}
		}
	}

	void draw(ShaderProgram& shader) {
		glMesh.draw(shader);
	}
};

#endif