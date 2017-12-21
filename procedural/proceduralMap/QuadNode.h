#ifndef QUAD_NODE_H
#define QUAD_NODE_H

#include "MapVertexType.h"
#include "QuadAllocator.h"

#include "Util.h"

/// fix normals 
/// solve texturing
/// Optimization minLevel ...
/// Optimize QuadAllocator drawing

class QuadID {
public:
	static int count;
	const int id;

	QuadID() : id(count++) {}

	// template <typename Type>
	// operator Type() {
	// 	return id;
	// }

	std::string toString() {
		std::stringstream ss;
		ss << id;

		return ss.str();
	}

	friend std::ostream& operator << (std::ostream& stream, const QuadID& arg) {
		stream << arg.id;

		return stream;
	}
};
int QuadID::count = 0;

template <typename ProcChoser>
class QuadNode : public std::enable_shared_from_this<QuadNode<ProcChoser>> {
public:
	std::reference_wrapper<QuadAllocator> allocator;

	// for debug only?
	QuadID id;

	/// vertexes of the node
	// 0 1
	// 2 3
	std::vector <int> vertexes; /// needs pointers to vertexes	   
	int element;

	std::shared_ptr <QuadNode> parent;
	std::shared_ptr <QuadNode> thisNode;
	std::shared_ptr <QuadNode> left, right, top, bottom; 

	/// the 9 childs of the node
	std::vector <std::shared_ptr <QuadNode>> childs;

	Point2i start;
	int level;
	bool isSplit = false;
	bool isComplete = false;

	static const int tl = 0;	// top left 
	static const int tr = 1;	// top right
	static const int bl = 2;	// bottom left
	static const int br = 3;	// bottom right

	static const int INVALID_ID = -1;
	static int quadCount;

	QuadNode (int level, QuadAllocator &allocator, Point2i start) 
				: vertexes(4), childs(9), allocator(allocator), start(start), level(level)
	{
		vertexes[tl] = INVALID_ID;
		vertexes[tr] = INVALID_ID;
		vertexes[bl] = INVALID_ID;
		vertexes[br] = INVALID_ID;

		element = INVALID_ID;
		quadCount++;
	}

	~QuadNode() {
		quadCount--;
	}

	bool hasNeighs() {
		return bool(top) && bool(bottom) && bool(left) && bool(right);
	}

	/// really needs optimizing...
	// merges nodes outside of size and splits nodes inside of size
	void recUpdate (int size, Point2f pos) {
		if (testQuad(size, pos, start)) {
			merge();
		}
		else {
			if (!isSplit)
				split();

			for (auto&& child : childs) {
				if (child)
					child->recUpdate(size, pos);
			}
		}
	}
	/// at node level n we want to know if there is a level k such that k < n and 
	/// there is at least a node at level k that needs merging or at least a node that
	/// needs spliting
	/// merges ocour if testQuad(const size, const pos, quad start) is true, so quad is 
	/// outside the square at that level or else a split ocurs

	/// really needs optimizing...
	void fix() {
		if (isComplete) {
			if (!hasNeighs()) {
				fixVertsAvrg();
			}
		}
		if (!isComplete) {
			if (hasNeighs()) {
				fixVertsRand();
			}
		}
		
		for (auto&& child : childs)
			if (child)
				child->fix();
	}

	void update(int size, Point2f pos) {
		recUpdate(size, pos);
		for (auto&& child : childs)
			if (child)
				child->fix();
	}

	bool testQuad (int size, Point2f pos, Point2i start) {
		int side = pow(3, level);
		int squareDist = sqrt(size);
		// DEBUG("squareDist * side: " << squareDist * side);

		auto center = getCenterPos(start);
		auto vec1 = Point2i((int)pos.x, (int)pos.y);
		auto vec2 = Point2i((int)center.x, (int)center.y);
		
		return std::abs(vec1.x - vec2.x) / 2 > squareDist * side 
				|| std::abs(vec1.y - vec2.y) / 2 > squareDist * side;
	}

	// void recalcMinSplit() {
	// 	int lastMinSplit = minSplitLevel;
	// 	if (isSplit) {
	// 		minSplitLevel = childs[0]->minSplitLevel + 1;
	// 		for (auto&& child : childs)
	// 			minSplitLevel = std::min(child->minSplitLevel + 1, minSplitLevel);

	// 		if (lastMinSplit != minSplitLevel)
	// 			if (parent)
	// 				parent->recalcMinSplit();
	// 	}
	// }

	/// will get the pos of this quad
	Point2f getCenterPos () {
		int sideLength = pow(3, level);
		return Point2f(start.x, start.y) + Point2f(sideLength / 2.0, sideLength / 2.0);
	}

	/// will get the pos of other quad with start start
	Point2f getCenterPos (Point2i start) {
		int sideLength = pow(3, level);
		return Point2f(start.x, start.y) + Point2f(sideLength / 2.0, sideLength / 2.0);
	}


	//		tl	tr
	//	tl	tl	tr	tr
	//	bl	bl	br	br
	//		bl	br

	void fixNormal (int index) {
		Point3f normalResult = Point3f(0);

		bool count[4] = {
			(bool)top + (bool)left + (top && left) + 1,			// tl
			(bool)top + (bool)right + (top && right) + 1,			// tr
			(bool)bottom + (bool)left + (bottom && left) + 1,		// bl
			(bool)bottom + (bool)right + (bottom && right) + 1	// br
		};

		auto tl_point3f = allocator.get().getVert(level, vertexes[tl]).get<VertexPosition>();
		auto tr_point3f = allocator.get().getVert(level, vertexes[tr]).get<VertexPosition>();
		auto bl_point3f = allocator.get().getVert(level, vertexes[bl]).get<VertexPosition>();
		auto br_point3f = allocator.get().getVert(level, vertexes[br]).get<VertexPosition>();

		auto t_tl_point3f = Point3f(0);
		auto t_tr_point3f = Point3f(0);

		auto r_tr_point3f = Point3f(0);
		auto r_br_point3f = Point3f(0);
		
		auto l_tl_point3f = Point3f(0);
		auto l_bl_point3f = Point3f(0);
		
		auto b_bl_point3f = Point3f(0);
		auto b_br_point3f = Point3f(0);

		if (top) {				
			t_tl_point3f = allocator.get().getVert(level, top->vertexes[tl]).template get<VertexPosition>();
			t_tr_point3f = allocator.get().getVert(level, top->vertexes[tr]).template get<VertexPosition>();
		}

		if (right) {
			r_tr_point3f = allocator.get().getVert(level, right->vertexes[tr]).template get<VertexPosition>();
			r_br_point3f = allocator.get().getVert(level, right->vertexes[br]).template get<VertexPosition>();
		}

		if (left) {
			l_tl_point3f = allocator.get().getVert(level, left->vertexes[tl]).template get<VertexPosition>();
			l_bl_point3f = allocator.get().getVert(level, left->vertexes[bl]).template get<VertexPosition>();
		}

		if (bottom) {
			b_bl_point3f = allocator.get().getVert(level, bottom->vertexes[bl]).template get<VertexPosition>();
			b_br_point3f = allocator.get().getVert(level, bottom->vertexes[br]).template get<VertexPosition>();
		}

		Point3f verts[4][4] = {
			{Point3f(0),	t_tl_point3f,	t_tr_point3f,	Point3f(0)},
			{l_tl_point3f,	tl_point3f,		tr_point3f,		r_tr_point3f},
			{l_bl_point3f,	bl_point3f,		br_point3f,		r_br_point3f},
			{Point3f(0),	b_bl_point3f,	b_br_point3f,	Point3f(0)}
		};

		bool vert_is[4][4] = {
			{false,			(bool)top,		(bool)top,		false},
			{(bool)left,	true,			true,			(bool)right},
			{(bool)left,	true,			true,			(bool)right},
			{false,			(bool)bottom,	(bool)bottom,	false}
		};		

		//					tl,	tr,	bl,	br
		int index_r[] = {	1,	1,	2,	2};
		int index_c[] = {	1,	2,	1,	2};

		auto center = verts[index_r[index]][index_c[index]];

		auto crossCoord = [&](int x1, int y1, int x2, int y2) {
			return (Point3f(verts[index_r[index] + x1][index_c[index] + y1] - center)).cross(
					Point3f(verts[index_r[index] + x2][index_c[index] + y2] - center)) * 
							(vert_is[index_r[index] + x1][index_c[index] + y1] && 
							vert_is[index_r[index] + x2][index_c[index] + y2]);
		}; 


		Point3f norm1 = crossCoord(-1,	0,	0,	-1	);	// (top with left) 
		Point3f norm2 = crossCoord(0,	-1,	1,	0	);	// (left with bottom) 
		Point3f norm3 = crossCoord(1,	0,	0,	1	);	// (bottom with right) 
		Point3f norm4 = crossCoord(0,	1,	-1,	0	);	// (right with top) 

		allocator.get().getVert(level, vertexes[index]).get<VertexNormal>() =
				-1 * ((norm1 + norm2 + norm3 + norm4) / float(count[index])).normalize();
	}

	void fixNormals (std::initializer_list<int> index_list) {
		for (auto&& index : index_list) {
			fixNormal (index);
		}
	}

	/// Choses attrib for vertexes, those vertexes ARE on the edge of the level
	void fixVertsAvrg() {
		auto fixer = [this](int level, int child, int firstParent, int secondParent) {
			allocator.get().getVert(level, child).get<VertexPosition>().y =
			allocator.get().getVert(level + 1, firstParent).get<VertexPosition>().y * 0.666666 +
			allocator.get().getVert(level + 1, secondParent).get<VertexPosition>().y * 0.333334;

			// allocator.get().getVert(level, child).get<VertexNormal>() =
			// allocator.get().getVert(level + 1, firstParent).get<VertexNormal>() * 0.666666 +
			// allocator.get().getVert(level + 1, secondParent).get<VertexNormal>() * 0.333334;

			allocator.get().getVert(level, child).get<VertexColor>() =
			allocator.get().getVert(level + 1, firstParent).get<VertexColor>() * 0.666666 +
			allocator.get().getVert(level + 1, secondParent).get<VertexColor>() * 0.333334;
		};

		/// 0 1 2
		/// 3 4 5
		/// 6 7 8
		if (parent) {
			if (!top) {
				fixer(level, parent->childs[1]->vertexes[tl], parent->vertexes[tl], parent->vertexes[tr]);
				fixer(level, parent->childs[1]->vertexes[tr], parent->vertexes[tr], parent->vertexes[tl]);
				parent->childs[1]->fixNormal(tl);
				parent->childs[1]->fixNormal(tr);
				parent->childs[0]->isComplete = false;
				parent->childs[1]->isComplete = false;
				parent->childs[2]->isComplete = false;
			}
			if (!left) {
				fixer(level, parent->childs[3]->vertexes[tl], parent->vertexes[tl], parent->vertexes[bl]);
				fixer(level, parent->childs[3]->vertexes[bl], parent->vertexes[bl], parent->vertexes[tl]);
				parent->childs[3]->fixNormal(tl);
				parent->childs[3]->fixNormal(bl);
				parent->childs[0]->isComplete = false;
				parent->childs[3]->isComplete = false;
				parent->childs[6]->isComplete = false;
			}
			if (!bottom) {
				fixer(level, parent->childs[7]->vertexes[br], parent->vertexes[br], parent->vertexes[bl]);
				fixer(level, parent->childs[7]->vertexes[bl], parent->vertexes[bl], parent->vertexes[br]);
				parent->childs[7]->fixNormal(br);
				parent->childs[7]->fixNormal(bl);
				parent->childs[6]->isComplete = false;
				parent->childs[7]->isComplete = false;
				parent->childs[8]->isComplete = false;
			}
			if (!right) {
				fixer(level, parent->childs[5]->vertexes[tr], parent->vertexes[tr], parent->vertexes[br]);
				fixer(level, parent->childs[5]->vertexes[br], parent->vertexes[br], parent->vertexes[tr]);
				parent->childs[5]->fixNormal(tr);
				parent->childs[5]->fixNormal(br);
				parent->childs[2]->isComplete = false;
				parent->childs[5]->isComplete = false;
				parent->childs[8]->isComplete = false;
			}
		}
	}

	/// Choses attrib for vertexes, those vertexes ARE NOT on the edge of the level
	void fixVertsRand() {
		if (parent) {
			double var1 = 0.67;
			double var2 = 0.33;
			double var3 = 0.25;
			double var4 = 0.17;

			/// contributions
			double contrib[4][4][4] = {
				// tl
				{
					{1.00, var1, var2, 0.00},
					{var1, var2, var3, 0.00},
					{var2, var3, var4, 0.00},
					{0.00, 0.00, 0.00, 0.00}
				},
				// tr
				{
					{0.00, var2, var1, 1.00},
					{0.00, var3, var2, var1},
					{0.00, var4, var3, var2},
					{0.00, 0.00, 0.00, 0.00}
				},
				// bl
				{
					{0.00, 0.00, 0.00, 0.00},
					{var2, var3, var4, 0.00},
					{var1, var2, var3, 0.00},
					{1.00, var1, var2, 0.00}
				},
				// br
				{
					{0.00, 0.00, 0.00, 0.00},
					{0.00, var4, var3, var2},
					{0.00, var3, var2, var1},
					{0.00, var2, var1, 1.00}
				},
			};
			auto& pCh = parent->childs; 
			int pTL = parent->vertexes[tl];
			int pTR = parent->vertexes[tr];
			int pBL = parent->vertexes[bl];
			int pBR = parent->vertexes[br];
			/// 0 1 2
			/// 3 4 5
			/// 6 7 8
			int newVerts[4][4] = {
				{-1					 , pCh[1]->vertexes[tl], pCh[1]->vertexes[tr], -1				   },
				{pCh[3]->vertexes[tl], pCh[4]->vertexes[tl], pCh[4]->vertexes[tr], pCh[5]->vertexes[tr]},
				{pCh[3]->vertexes[bl], pCh[4]->vertexes[bl], pCh[4]->vertexes[br], pCh[5]->vertexes[br]},
				{-1					 , pCh[7]->vertexes[bl], pCh[7]->vertexes[br], -1				   }
			};
			int childSide = pow(3, level);
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					if (newVerts[i][j] != -1) {
						allocator.get().getVert(level, newVerts[i][j]) = ProcChoser::getVertParent(
							parent->start + Point2i(j, i) * childSide, 
							level
						);

						/// positions
						allocator.get().getVert(level, newVerts[i][j]).get<VertexPosition>().y =
						allocator.get().getVert(level, newVerts[i][j]).get<VertexPosition>().y + 
						allocator.get().getVert(level + 1, pTL).get<VertexPosition>().y * contrib[tl][i][j] +
						allocator.get().getVert(level + 1, pTR).get<VertexPosition>().y * contrib[tr][i][j] +
						allocator.get().getVert(level + 1, pBL).get<VertexPosition>().y * contrib[bl][i][j] +
						allocator.get().getVert(level + 1, pBR).get<VertexPosition>().y * contrib[br][i][j];
						
						// /// normals
						// allocator.get().getVert(level, newVerts[i][j]).get<VertexNormal>() =
						// (allocator.get().getVert(level, newVerts[i][j]).get<VertexNormal>() + 
						// allocator.get().getVert(level + 1, pTL).get<VertexNormal>() * contrib[tl][i][j] +
						// allocator.get().getVert(level + 1, pTR).get<VertexNormal>() * contrib[tr][i][j] +
						// allocator.get().getVert(level + 1, pBL).get<VertexNormal>() * contrib[bl][i][j] +
						// allocator.get().getVert(level + 1, pBR).get<VertexNormal>() * contrib[br][i][j]) / 2.0f;

						// allocator.get().getVert(level, newVerts[i][j]).get<VertexNormal>() =
						// allocator.get().getVert(level, newVerts[i][j]).get<VertexNormal>().normalize();

						/// colors
						allocator.get().getVert(level, newVerts[i][j]).get<VertexColor>() =
						allocator.get().getVert(level, newVerts[i][j]).get<VertexColor>();
					}
				}
			}
			for (auto&& child : parent->childs) {
				child->isComplete = true;
			}

			/// 0 1 2
			/// 3 4 5
			/// 6 7 8
			parent->childs[0]->fixNormals({tl, tr, bl, br});
			parent->childs[2]->fixNormals({tl, tr, bl, br});
			parent->childs[6]->fixNormals({tl, tr, bl, br});
			parent->childs[8]->fixNormals({tl, tr, bl, br});
		}
	}

	// creates the 9 childs, relinks them and allocs geometry
	void split() {
		if (isSplit || level <= 0)
			return;

		isSplit = true;

		// if (parent)
		// 	parent->recalcMinSplit();

		/// first create childs and after that link them 		
		int childSide = pow(3, level - 1);

		for (int i = 0; i < 9; i++) {
			childs[i] = std::shared_ptr<QuadNode>(new QuadNode(level - 1, allocator
					, start + Point2i(i % 3, i / 3) * childSide));
			childs[i]->parent = std::enable_shared_from_this<QuadNode>::shared_from_this();
		}

		const static int first = 0, second = 1;

		/// childs:
		///    _6_7_8_	
		///
		/// _2  0 1 2  0_
		/// _5  3 4 5  3_
		/// _8  6 7 8  6_
		///
		///    _0_1_2_

		/// external links:

		if (top && top->isSplit) {

			int pairs[3][2] = {{0, 6}, {1, 7}, {2, 8}};

			for (auto&& pair : pairs) {
				childs[pair[first]]->top = top->childs[pair[second]];
				top->childs[pair[second]]->bottom = childs[pair[first]];	
			}
		}

		if (left && left->isSplit) {
			int pairs[3][2] = {{0, 2}, {3, 5}, {6, 8}};

			for (auto&& pair : pairs) {
				childs[pair[first]]->left = left->childs[pair[second]];
				left->childs[pair[second]]->right = childs[pair[first]];	
			}
		}

		if (bottom && bottom->isSplit) {
			int pairs[3][2] = {{6, 0}, {7, 1}, {8, 2}};

			for (auto&& pair : pairs) {
				childs[pair[first]]->bottom = bottom->childs[pair[second]];
				bottom->childs[pair[second]]->top = childs[pair[first]];
			}
		}

		if (right && right->isSplit) {
			int pairs[3][2] = {{2, 0}, {5, 3}, {8, 6}};

			for (auto&& pair : pairs) {
				childs[pair[first]]->right = right->childs[pair[second]];
				right->childs[pair[second]]->left = childs[pair[first]];
			}
		}

		/// internal links:

		/// childs:
		///
		///	 0 1 2 
		///  3 4 5 
		///  6 7 8 

		/// of the form "first has second in right"
		static int rights[][2] = {
			{0, 1}, {1, 2},
			{3, 4}, {4, 5},
			{6, 7}, {7, 8}
		};

		/// of the form "first has second in left"
		static int lefts[][2] = {
			{1, 0}, {2, 1},
			{4, 3}, {5, 4},
			{7, 6}, {8, 7}
		};

		/// of the form "first has second on top"
		static int tops[][2] = {
			{3, 0}, {4, 1}, {5, 2},
			{6, 3}, {7, 4}, {8, 5}
		};

		/// of the form "first has second on bottom"
		static int bottoms[][2] = {
			{0, 3}, {1, 4}, {2, 5},
			{3, 6}, {4, 7}, {5, 8}
		};

		for (auto&& pair : rights)
			childs[pair[first]]->right = childs[pair[second]];

		for (auto&& pair : lefts)
			childs[pair[first]]->left = childs[pair[second]];

		for (auto&& pair : bottoms)
			childs[pair[first]]->bottom = childs[pair[second]];

		for (auto&& pair : tops)
			childs[pair[first]]->top = childs[pair[second]];

		/// verts on childs:
		/// 0:0  0:1|  0:1  0:2|  0:2  0:3
		/// 1:0  1:1|  1:1  1:2|  1:2  1:3
		///------------------------
		/// 1:0  1:1|  1:1  1:2|  1:2  1:3
		/// 2:0  2:1|  2:1  2:2|  2:2  2:3
		///------------------------
		/// 2:0  2:1|  2:1  2:2|  2:2  2:3
		/// 3:0  3:1|  3:1  3:2|  3:2  3:3

		int newVerts[4][4] = {
			{vertexes[tl], -1, -1,  vertexes[tr]},
			{-1, 		   -1, -1, -1			},
			{-1, 		   -1, -1, -1			},
			{vertexes[bl], -1, -1,  vertexes[br]}
		};

		if (top && top->isSplit) {
			newVerts[0][1] = top->childs[7]->vertexes[bl];
			newVerts[0][2] = top->childs[7]->vertexes[br];
		}

		if (left && left->isSplit) {
			newVerts[1][0] = left->childs[5]->vertexes[tr];
			newVerts[2][0] = left->childs[5]->vertexes[br];
		}

		if (bottom && bottom->isSplit) {
			newVerts[3][1] = bottom->childs[1]->vertexes[tl];
			newVerts[3][2] = bottom->childs[1]->vertexes[tr];
		}

		if (right && right->isSplit) {
			newVerts[1][3] = right->childs[3]->vertexes[tl];
			newVerts[2][3] = right->childs[3]->vertexes[bl];
		}

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				if (newVerts[i][j] == -1)
					newVerts[i][j] = allocator.get().allocVert(level - 1);

		/// putting the respective vertexes in place
		for (int i = 0; i < 9; i++) {
			childs[i]->vertexes[tl] = newVerts[i / 3 + 0][i % 3 + 0];
			childs[i]->vertexes[tr] = newVerts[i / 3 + 0][i % 3 + 1];
			childs[i]->vertexes[bl] = newVerts[i / 3 + 1][i % 3 + 0];
			childs[i]->vertexes[br] = newVerts[i / 3 + 1][i % 3 + 1];
			childs[i]->isComplete = true; 
		}

		/// actually chose vertex, colors, normals, texcoords, etc...
		childs[4]->fixVertsRand();
		for (int i = 0; i < 9; i++) {
			childs[i]->fixNormal(tl);
		}
		childs[0]->fixNormals({tl, tr, bl, br});
		childs[2]->fixNormals({tl, tr, bl, br});
		childs[6]->fixNormals({tl, tr, bl, br});
		childs[8]->fixNormals({tl, tr, bl, br});

		/// deleting old face
		if (element != INVALID_ID) {
			allocator.get().getElem(level, element) = std::vector<int>{0, 0, 0, 0};
			allocator.get().freeElem(level, element);
			element = INVALID_ID;
		}

		/// making each new face
		for (auto&& child: childs) {
			if (child->element == INVALID_ID) {
				child->element = allocator.get().allocElem(level - 1);
				allocator.get().getElem(level - 1, child->element) = std::vector<int>{
					child->vertexes[tl],
					child->vertexes[tr],
					child->vertexes[br],
					child->vertexes[bl]
				};
			}
		}

		/// done spliting this square
	}

	// deletes the 9 childs after merging them, erases links ans deletes geometry
	void merge() {
		if (!isSplit || level <= 0)
			return;

		// DEBUG("MERGE ... ");

		isSplit = false;

		/// this recalc might need some optimiztions 
		// if (parent)
		// 	parent->recalcMinSplit();

		/// marge the child of childs
		for (auto&& child : childs) {
			if (child)
				child->merge();
		}

		/// delete the vertexes with no more links
		for (auto&& child : childs) {
			if (child) {
				/// free tl
				if ((child->top && child->top->vertexes[bl] != INVALID_ID) ||
						(child->left && child->left->vertexes[tr] != INVALID_ID)) 
				{
					if (child != childs[0])
						child->vertexes[tl] = INVALID_ID;
				}
				else {
					if (child != childs[0] && child->vertexes[tl] != INVALID_ID) {
						allocator.get().freeVert(level - 1, child->vertexes[tl]);
						child->vertexes[tl] = INVALID_ID;		
					}
				}

				/// free tr
				if ((child->top && child->top->vertexes[br] != INVALID_ID) ||
						(child->right && child->right->vertexes[tl] != INVALID_ID)) 
				{
					if (child != childs[2])
						child->vertexes[tr] = INVALID_ID;
				}
				else {
					if (child != childs[2] && child->vertexes[tr] != INVALID_ID) {
						allocator.get().freeVert(level - 1, child->vertexes[tr]);
						child->vertexes[tr] = INVALID_ID;		
					}
				}

				/// free bl
				if ((child->bottom && child->bottom->vertexes[tl] != INVALID_ID) ||
						(child->left && child->left->vertexes[br] != INVALID_ID)) 
				{
					if (child != childs[6])
						child->vertexes[bl] = INVALID_ID;
				}
				else {
					if (child != childs[6] && child->vertexes[bl] != INVALID_ID) {
						allocator.get().freeVert(level - 1, child->vertexes[bl]);
						child->vertexes[bl] = INVALID_ID;		
					}
				}

				/// free br
				if ((child->bottom && child->bottom->vertexes[tr] != INVALID_ID) ||
						(child->right && child->right->vertexes[bl] != INVALID_ID)) 
				{
					if (child != childs[8])
						child->vertexes[br] = INVALID_ID;
				}
				else {
					if (child != childs[8] && child->vertexes[br] != INVALID_ID) {
						allocator.get().freeVert(level - 1, child->vertexes[br]);
						child->vertexes[br] = INVALID_ID;		
					}
				}
			}
		}

		/// delete the faces of the childs
		for (auto&& child : childs) {
			if (child) {
				if (child->element != INVALID_ID) {
					allocator.get().getElem(level - 1, child->element) = std::vector<int>{0, 0, 0, 0};
					allocator.get().freeElem(level - 1, child->element);
					child->element = INVALID_ID;
				}
			}
		}

		/// re-making the big face
		if (element == INVALID_ID) {
			element = allocator.get().allocElem(level);
			allocator.get().getElem(level, element) = std::vector<int>{
				vertexes[tl],
				vertexes[tr],
				vertexes[br],
				vertexes[bl]
			};
		}

		/// free all the links of childs 
		for (auto&& child : childs) {
			if (child) {
				if (child->top) {
					child->top->bottom = nullptr;
					child->top = nullptr;
				}

				if (child->left) {
					child->left->right = nullptr;
					child->left = nullptr;
				}

				if (child->bottom) {
					child->bottom->top = nullptr;
					child->bottom = nullptr;
				}

				if (child->right) {
					child->right->left = nullptr;
					child->right = nullptr;
				}
			}
		}

		/// free all the links 
		for (auto&& child : childs) {
			if (child) {
				child->parent = nullptr;
				child = nullptr;
			}
		}
	}

	// this func is only usefull for roots (quads with no parents)
	//
	//		 tbl tbr
	//   ltr tl  tr  rtl
	//   lbr bl  br  rbl
	//       btl btr
	//
	void initNoParent() {
		std::vector<int*> groupTL;
		std::vector<int*> groupTR;
		std::vector<int*> groupBL;
		std::vector<int*> groupBR;

		groupTL.push_back(&(vertexes[tl]));
		groupTR.push_back(&(vertexes[tr]));
		groupBL.push_back(&(vertexes[bl]));
		groupBR.push_back(&(vertexes[br]));

		if (top) {
			groupTL.push_back(&(top->vertexes[bl]));
			groupTR.push_back(&(top->vertexes[br]));
		}

		if (right) {
			groupTR.push_back(&(right->vertexes[tl]));
			groupBR.push_back(&(right->vertexes[bl]));
		}

		if (bottom) {
			groupBL.push_back(&(bottom->vertexes[tl]));
			groupBR.push_back(&(bottom->vertexes[tr]));
		}

		if (left) {
			groupTL.push_back(&(left->vertexes[tr]));
			groupBL.push_back(&(left->vertexes[br]));
		}

		auto choseID = [this](std::vector<int*>& vec, Point2i corner) {
			int chosenID = INVALID_ID;
			for (auto&& elem : vec) {
				if (chosenID != INVALID_ID && *elem != -1 && *elem != chosenID)
					std::cout << "ERROR CHOSING MAP ID" << std::endl;
				
				if (*elem != -1 && chosenID == INVALID_ID)
					chosenID = *elem;
			}

			if (chosenID != INVALID_ID) {
				for (auto&& elem : vec)
					*elem = chosenID;
			}
			else {
				chosenID = allocator.get().allocVert(level);
				allocator.get().getVert(level, chosenID)
						= ProcChoser::getVertParent(start + corner, level);
				for (auto&& elem : vec)
					*elem = chosenID;	
			}
		};

		int sideLen = pow(3, level);
		choseID(groupTL, Point2i(0, 0));
		choseID(groupTR, Point2i(sideLen, 0));
		choseID(groupBL, Point2i(0, sideLen));
		choseID(groupBR, Point2i(sideLen, sideLen));

		if (element == INVALID_ID && !isSplit) {
			element = allocator.get().allocElem(level);
			allocator.get().getElem(level, element) = std::vector<int>{
				vertexes[tl],
				vertexes[tr],
				vertexes[br],
				vertexes[bl]
			};
		}
	}

	// this func is only usefull for roots (quads with no parrent)
	//
	//		 tbl tbr
	//   ltr tl  tr  rtl
	//   lbr bl  br  rbl
	//       btl btr
	//
	void eraseNoParent() {
		if (isSplit) {
			merge();
		}

		if (element != INVALID_ID) {
			allocator.get().getElem(level, element) = std::vector<int>{0, 0, 0, 0};
			allocator.get().freeElem(level, element);
			element = INVALID_ID;
		}

		if ((top && top->vertexes[bl] != INVALID_ID) || 
				(left && left->vertexes[tr] != INVALID_ID))
		{
			vertexes[tl] = INVALID_ID;
		}
		else {
			if (vertexes[tl] != INVALID_ID)
				allocator.get().freeVert(level, vertexes[tl]);
			vertexes[tl] = INVALID_ID;
		}

		if ((top && top->vertexes[br] != INVALID_ID) || 
				(right && right->vertexes[tl] != INVALID_ID))
		{
			vertexes[tr] = INVALID_ID;
		}
		else {
			if (vertexes[tr] != INVALID_ID)
				allocator.get().freeVert(level, vertexes[tr]);
			vertexes[tr] = INVALID_ID;
		}

		if ((bottom && bottom->vertexes[tl] != INVALID_ID) || 
				(left && left->vertexes[br] != INVALID_ID))
		{
			vertexes[bl] = INVALID_ID;
		}
		else {
			if (vertexes[bl] != INVALID_ID)
				allocator.get().freeVert(level, vertexes[bl]);
			vertexes[bl] = INVALID_ID;
		}

		if ((bottom && bottom->vertexes[tr] != INVALID_ID) || 
				(right && right->vertexes[bl] != INVALID_ID))
		{
			vertexes[br] = INVALID_ID;
		}
		else {
			if (vertexes[br] != INVALID_ID)
				allocator.get().freeVert(level, vertexes[br]);
			vertexes[br] = INVALID_ID;
		}
	}

	void debugDraw() {
		if (element != INVALID_ID) {
			auto vertTL = vertexes[tl] != INVALID_ID ? 
					allocator.get().getVert(level, vertexes[tl]) : MapVertexType();
			
			auto vertTR = vertexes[tr] != INVALID_ID ? 
					allocator.get().getVert(level, vertexes[tr]) : MapVertexType();
			
			auto vertBL = vertexes[bl] != INVALID_ID ? 
					allocator.get().getVert(level, vertexes[bl]) : MapVertexType();
			
			auto vertBR = vertexes[br] != INVALID_ID ? 
					allocator.get().getVert(level, vertexes[br]) : MapVertexType();

			glBegin(GL_QUADS);
				glColor3f(1, 0, 0);
				glVertex3fv(vertTL.get<VertexPosition>().array);
				glVertex3fv(vertTR.get<VertexPosition>().array);
				glVertex3fv(vertBR.get<VertexPosition>().array);
				glVertex3fv(vertBL.get<VertexPosition>().array);
			glEnd();
		}

		if (level > 0)
			for (auto&& child : childs) 
				if (child)
					child->debugDraw();
	}
};

template <typename Type>
int QuadNode<Type>::quadCount = 0;

#endif