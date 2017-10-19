#ifndef PROCEDURALMAPP_H_INCLUDED
#define PROCEDURALMAPP_H_INCLUDED 

#include "Mesh.h"
#include <memory>

class ProceduralMap {
public:
	typedef Vertex <
		VertexAttribute <
			int, 		VertexTexID				, VertexAttribute <
			Point2f, 	VertexTexCoord			, VertexAttribute <
			Point3f, 	VertexNormal			, VertexAttribute <
			Point3f, 	VertexPosition			, VertexAttributeEnd
			// Point4f, 	VertexColor				, VertexAttributeEnd	
	> > > > >
	MapVertexType;

	class QuadAllocator {
	public: 
		static MapVertexType emptyVertex;
		std::shared_ptr<MapVertexType> MapVertexArray;
		int size;
		int stackLast = 0;

		QuadAllocator (int size) : size(size) {
			/// add dealocator 
			MapVertexArray = std::shared_ptr<MapVertexType> (new MapVertexType[size], 
									Util::array_deleter<MapVertexType>());			
		}

		int addVert(const MapVertexType& toAdd) {
			if (stackLast != size - 1) {
				MapVertexArray.get()[stackLast++] = toAdd;
				return stackLast;
			}
			else {
				return -1;
			}
		}

		MapVertexType& get (int id) {
			if (id >= 0 && id <= stackLast)
				return MapVertexArray.get()[id];
			else
				return emptyVertex;
		}
	};

	class QuadNode {
	public:
		QuadAllocator &alocator;

		/// vertexes of the node
		std::vector <int> vertexes; /// needs pointers to vertexes	   

		std::shared_ptr <QuadNode> parent; 
		std::shared_ptr <QuadNode> left, right, top, bottom; 

		/// the 9 childs of the node
		std::vector <std::shared_ptr <QuadNode>> childs;  

		QuadNode (Point3f A, Point3f B, Point3f C, QuadAllocator &alocator) 
					: vertexes(4), childs(9), alocator(alocator)
		{
			/// A ... B
			/// D ... C 
			Point3f D = C + A - B;

			alocator.get(vertexes[0] = alocator.addVert(MapVertexType())).get<VertexPosition>() = A;
			alocator.get(vertexes[1] = alocator.addVert(MapVertexType())).get<VertexPosition>() = B;
			alocator.get(vertexes[2] = alocator.addVert(MapVertexType())).get<VertexPosition>() = C;
			alocator.get(vertexes[3] = alocator.addVert(MapVertexType())).get<VertexPosition>() = D; 
		}

		// QuadNode (int A, int B, int C, int D)

		void split() {
			// childs[0] = std::shared_ptr<QuadNode>(new QuadNode());			
		}

		void merge() {

		}
	};

	QuadNode root;
	QuadAllocator alocator; 

	ProceduralMap (int size, Point3f A, Point3f B, Point3f C) : alocator(size), root(A, B, C, alocator) {}

	// code .. 
};


#endif