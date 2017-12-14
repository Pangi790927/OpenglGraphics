#ifndef PROCEDURAL_CHOSER_H
#define PROCEDURAL_CHOSER_H

#include "MapVertexType.h"

class DefaultProceduralChoser {
public:
	static int hash (int a, int b) {
		srand(a);
		int res1 = rand();

		srand(b * res1);
		int res2 = rand();
		
		srand(res1 * res2);
		
		return rand();
	}
	
	static int hash (Point2i pos) {
		return hash(pos.x, pos.y);
	}

	const static int MAX_HASH = RAND_MAX;

	static MapVertexType getRandVert(Point2i pos, int level) {

		int result1 = hash(Point2i(pos.x, pos.y));
		int result2 = hash(Point2i(pos.y, pos.x));

		auto genNext = [&]() {
			int aux = result1;
			
			result1 = result2;
			result2 = hash(aux, result2);

			return result1;
		};

		double r = genNext() / double(MAX_HASH);
		double g = genNext() / double(MAX_HASH);
		double b = genNext() / double(MAX_HASH);

		r = b = 0;
		g = 1; 

		double height = (genNext() / double(MAX_HASH)) * 2.0f - 1.0f;

		MapVertexType vert;

		if (level <= 3) {
			vert.get<VertexPosition>() = Point3f(pos.x, height * pow(3, level) * 0.1, pos.y);
			vert.get<VertexColor>() = Point3f(r * 0.4, g, b * 0.4) * 0.3;
		}
		else if (level <= 4) {
			vert.get<VertexPosition>() = Point3f(pos.x, height * pow(3, level) * 0.1 , pos.y);
			vert.get<VertexColor>() = Point3f(r * 0.4, g, b * 0.4) * 0.3;
		}
		else if (level <= 5) {
			vert.get<VertexPosition>() = Point3f(pos.x, height * pow(3, level) * 0.5 , pos.y);
			vert.get<VertexColor>() = Point3f(r * 0.4, g, b * 0.4) * 0.3;
		}
		else if (level <= 9) {
			vert.get<VertexPosition>() = Point3f(pos.x, height * pow(3, level) * 0.1 , pos.y);
			vert.get<VertexColor>() = Point3f(r * 0.4, g, b * 0.4) * 0.3;
		}
		else if (level <= 200) {
			vert.get<VertexPosition>() = Point3f(pos.x, height * pow(3, level) * 0.02, pos.y);
			vert.get<VertexColor>() = Point3f(r * 0.4, g, b * 0.4) * 0.3;
		}

		return vert;
	}

	static MapVertexType getVertParent (Point2i pos, int level) {
		int sideLength = pow(3, level);

		MapVertexType vertTop 		= getRandVert(pos + Point2i(0, -sideLength), level);
		MapVertexType vertBottom 	= getRandVert(pos + Point2i(0,  sideLength), level);
		MapVertexType vertLeft 		= getRandVert(pos + Point2i(-sideLength, 0), level);
		MapVertexType vertRight 	= getRandVert(pos + Point2i( sideLength, 0), level);
	
		MapVertexType vert = getRandVert(pos + Point2i(0, 0), level);

		Point3f O = vert.get<VertexPosition>();
		Point3f T = vertTop.get<VertexPosition>();
		Point3f B = vertBottom.get<VertexPosition>();
		Point3f L = vertLeft.get<VertexPosition>();
		Point3f R = vertRight.get<VertexPosition>();

		Point3f N1 = Point3f(T - O).cross(R - O).normalize();
		Point3f N2 = Point3f(R - O).cross(B - O).normalize();
		Point3f N3 = Point3f(B - O).cross(L - O).normalize();
		Point3f N4 = Point3f(L - O).cross(T - O).normalize();

		vert.get<VertexNormal>() = ((N1 + N2 + N3 + N4) / 4.0f).normalize();

		return vert;
	}
};

#endif