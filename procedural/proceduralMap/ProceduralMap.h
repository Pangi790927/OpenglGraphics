#ifndef PROCEDURALMAPP_H_INCLUDED
#define PROCEDURALMAPP_H_INCLUDED 

#include "Mesh.h"
#include <memory>
#include <queue>
#include <set>

#include "FixedFunctionMeshDraw.h"

#include "MapVertexType.h"
#include "QuadAllocator.h"
#include "QuadNode.h"
#include "ProceduralChoser.h"

template <typename ProcChoser = DefaultProceduralChoser>
class ProceduralMap {
public:
	using MapQuadNode = QuadNode<ProcChoser>;

	std::shared_ptr <MapQuadNode> root;
	QuadAllocator allocator;
	
	Point2f center = Point2f(0, 0);
	int size;
	int levels;
	int rootLevel;

	template <typename Func>
	void applyToNeighs (std::shared_ptr<MapQuadNode> node, Func& func) {
		/// we assume that all neighs are in place
		func(node->top);
		func(node->bottom);
		func(node->left);
		func(node->right);
		func(node->top->left);
		func(node->top->right);
		func(node->bottom->left);
		func(node->bottom->right);
	}

	template <typename Func>
	void applyToNeighs (std::shared_ptr<MapQuadNode> node, Func&& func) {
		applyToNeighs(node, func);
	}

	std::shared_ptr<MapQuadNode> getMinNode (Point2f pos, std::shared_ptr<MapQuadNode> current) {
		auto distance = [](Point2f pos, std::shared_ptr<MapQuadNode> node) {
			return Point2f(node->getCenterPos() - pos).abs();
		};

		float minDistance = distance(pos, current);
		std::shared_ptr<MapQuadNode> result = current;

		applyToNeighs(current, [&](std::shared_ptr<MapQuadNode> node) {
			if (node && distance(pos, node) < minDistance) {
				minDistance = distance(pos, node);
				result = node;
			}
		});

		return result;
	}

	void createTopBottom (std::shared_ptr<MapQuadNode> node) {
		int sideLength = pow(3, rootLevel);

		if (!node->top) {
			node->top = std::shared_ptr<MapQuadNode>(new MapQuadNode(rootLevel, allocator, 
					node->start + Point2i(0, -sideLength)));
			node->top->bottom = node;
		}

		if (!node->bottom) {
			node->bottom = std::shared_ptr<MapQuadNode>(new MapQuadNode(rootLevel, allocator, 
					node->start + Point2i(0, sideLength)));
			node->bottom->top = node;
		}
	}

	void createLeftRight (std::shared_ptr<MapQuadNode> node) {
		int sideLength = pow(3, rootLevel);

		if (!node->left) {
			node->left = std::shared_ptr<MapQuadNode>(new MapQuadNode(rootLevel, allocator, 
					node->start + Point2i(-sideLength, 0)));
			node->left->right = node;
		}

		if (!node->right) {
			node->right = std::shared_ptr<MapQuadNode>(new MapQuadNode(rootLevel, allocator, 
					node->start + Point2i(sideLength, 0)));
			node->right->left = node;
		}
	}

	void makeNeighs (std::shared_ptr<MapQuadNode> node) {
		createTopBottom(node);
		createLeftRight(node);
		
		//     T
		// 	   |
		//   L-N-R
		//     |
		//     B

		createLeftRight(node->top);
		createLeftRight(node->bottom);

		//  TL-T-TR
		// 	   |
		//   L-N-R
		//     |
		//  BL-B-BR

		node->top->left->bottom = node->left;
		node->left->top = node->top->left;

		node->top->right->bottom = node->right;
		node->right->top = node->top->right;

		node->bottom->right->top = node->right;
		node->right->bottom = node->bottom->right;

		node->bottom->left->top = node->left;
		node->left->bottom = node->bottom->left;

		//  TL-T-TR
		// 	 | | |
		//   L-N-R
		//   | | |
		//  BL-B-BR

		node->initNoParent();
		applyToNeighs(node, [&](std::shared_ptr<MapQuadNode> neighNode) {
			neighNode->initNoParent();
		});
	}

	void erase (std::shared_ptr<MapQuadNode> node) {
		if (node->top)
			node->top->bottom = nullptr;

		if (node->bottom)
			node->bottom->top = nullptr;

		if (node->left)
			node->left->right = nullptr;

		if (node->right)
			node->right->left = nullptr;
		
		node->eraseNoParent();
	}

	bool isInGroup (std::shared_ptr<MapQuadNode> group, std::shared_ptr<MapQuadNode> node) {
		bool inGroup = false;
		if (group == node)
			return true;

		applyToNeighs(group, [&](std::shared_ptr<MapQuadNode> grupNode) {
			if (grupNode == node)
				inGroup = true;
		});

		return inGroup;
	}

	constexpr static const float CHANGE_ROOT_TRESH = 4.5 / 6.;

	void update (Point2f pos) {
		auto newRoot = getMinNode(pos, root);

		int sideLength = pow(3, rootLevel);
		if (newRoot == root || Point3f(root->getCenterPos() - pos).abs() >=
				sideLength * (CHANGE_ROOT_TRESH))
		{
			makeNeighs(newRoot);
		
			std::vector<std::shared_ptr<MapQuadNode>> toErase;
			
			if (!isInGroup(newRoot, root))
				toErase.push_back(root);

			applyToNeighs(root, [&](std::shared_ptr<MapQuadNode> node) {
				if (!isInGroup(newRoot, node))
					toErase.push_back(node);
			});

			/// rather strange solution, the order in wich we erase the nodes makes a huge difference
			/// because the deleted corner needs to know that it can not delete the vevrtex in common
			/// with the newRoot group. Thats why we need to erase them in reverse
			for (int i = toErase.size() - 1; i >= 0 ; i--)
				erase(toErase[i]);

			root = newRoot;
		}
		root->update(size, pos);
		applyToNeighs(root, [&, this](std::shared_ptr<MapQuadNode> node){
			node->update(size, pos);
		});
	}

	void draw (ShaderProgram& shader) {
		// FixedFunctionMeshDraw::draw<MapVertexType>(allocator.mesh, shader);
		allocator.updateDrawing();
		allocator.draw(shader);
	}

	void initDraw() {
		allocator.initDraw();
	}

	void debugDraw() {
		root->debugDraw();
		applyToNeighs(root, [&, this](std::shared_ptr<MapQuadNode> node){
			node->debugDraw();
		});
	}

	/// levels - number of levels
	/// size - the squares from center for each level 
	ProceduralMap (int size, int levels, Point2f start)
			: size(size), levels(levels), rootLevel(levels - 1)
			, allocator(QuadAllocator::calcAllocSize(size), levels)
	{
		int sideLength = pow(3, rootLevel);
		Point2i realStart = Point2i(int(start.x) / sideLength, int(start.y) / sideLength) * sideLength;
		center = Point2f(realStart.x + sideLength / 2.0, realStart.y + sideLength / 2.0 );
		
		root = std::shared_ptr<MapQuadNode>(new MapQuadNode(rootLevel, allocator, realStart));

		makeNeighs(root);

		root->update(size, start);
		applyToNeighs(root, [&, this](std::shared_ptr<MapQuadNode> node){
			node->update(size, start);
		});
	}
};

#endif