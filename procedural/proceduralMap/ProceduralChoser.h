#ifndef PROCEDURAL_CHOSER_H
#define PROCEDURAL_CHOSER_H

#include "MapVertexType.h"
#include "RandGenerator.h"

class DefaultProceduralChoser {
public:
	static int hash (int a, int b) {
		namespace RG = RandGenerator;

		RG::srand(a);
		int res1 = RG::rand();

		RG::srand(b + res1);
		int res2 = RG::rand();
		
		RG::srand(res1 * res2);
		return RG::rand();
	}
	
	static int hash (Point2i pos) {
		return hash(pos.x, pos.y);
	}

	const static int MAX_HASH = RandGenerator::MAX_RAND;

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

		r = 180 / 255.;
		b = 207 / 255.;
		g = 250 / 255.; 

		double height = (genNext() / double(MAX_HASH)) * 2.0f - 1.0f;

		MapVertexType vert;

		if (level <= 1) {
			vert.get<VertexPosition>() = Point3f(pos.x, height * pow(3, level) * 0.001, pos.y);
			vert.get<VertexColor>() = Point3f(r, g, b);
		}
		if (level <= 2) {
			vert.get<VertexPosition>() = Point3f(pos.x, height * pow(3, level) * 0.01, pos.y);
			vert.get<VertexColor>() = Point3f(r, g, b);
		}
		else if (level <= 3) {
			vert.get<VertexPosition>() = Point3f(pos.x, height * pow(3, level) * 0.1, pos.y);
			vert.get<VertexColor>() = Point3f(r, g, b);
		}
		else if (level <= 4) {
			vert.get<VertexPosition>() = Point3f(pos.x, height * pow(3, level) * 0.1 , pos.y);
			vert.get<VertexColor>() = Point3f(r, g, b);
		}
		else if (level <= 5) {
			vert.get<VertexPosition>() = Point3f(pos.x, height * pow(3, level) * 0.06 , pos.y);
			vert.get<VertexColor>() = Point3f(r, g, b);
		}
		else if (level <= 6) {
			vert.get<VertexPosition>() = Point3f(pos.x, height * pow(3, level) * 0.01 , pos.y);
			vert.get<VertexColor>() = Point3f(r, g, b);
		}
		else if (level <= 7) {
			vert.get<VertexPosition>() = Point3f(pos.x, height * pow(3, level) * 0.003 , pos.y);
			vert.get<VertexColor>() = Point3f(r, g, b);
		}
		else if (level <= 9) {
			vert.get<VertexPosition>() = Point3f(pos.x, height * pow(3, level) * 0.003 , pos.y);
			vert.get<VertexColor>() = Point3f(r, g, b);
		}
		else if (level <= 200) {
			vert.get<VertexPosition>() = Point3f(pos.x, height * pow(3, level) * 0.003, pos.y);
			vert.get<VertexColor>() = Point3f(r, g, b);
		}

		return vert;
	}

	static MapVertexType getVertParent (Point2i pos, int level) {	
		return getRandVert(pos, level);
	}
};

#endif