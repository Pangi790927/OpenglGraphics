#include "MathLib.h"

int main(int argc, char const *argv[])
{
	using namespace MathLib;
	using namespace std;

	Matf <1000, 20> testDim;

	// std::cout << testDim << std::endl;

	Vecf <3> vec1 (1, 0, 0);
	Vecf <3> vec2 (0, 2, 0);
	Vecf <3> vec3 (0, 0, 4);

	Matf <3, 3> test(vec1, vec2, vec3);
	Matd <1, 1> test2(1.2f);
	Mati <1, 1> test3(1.2f);

	Vecf <3> vec21 (2, 2, 2);
	
	std::cout << test << std::endl;
	std::cout << test2 << std::endl;
	std::cout << test3 << std::endl;
	std::cout << "-----------------" << std::endl;
	
	std::cout << vec21 << std::endl;
	std::cout << "-----------------" << std::endl;
	std::cout << test * vec21 << std::endl;
	std::cout << "-----------------" << std::endl;
	std::cout << test * 4 << std::endl;
	std::cout << "-----------------" << std::endl;
	std::cout << test.row(0)[0] << std::endl;
	
	auto line = test.row(2);

	std::cout << test.row(0).getAsMatrix() << std::endl;
	std::cout << test.coll(0).getAsMatrix() << std::endl;

	std::cout << test << std::endl;

	test.swapLines(0, 2);

	std::cout << test << std::endl;

	std::cout << test.det() << std::endl;

	std::cout << MathLib::Matf<3, 3> (
		1, 2, 3,
		4, 5, 6,
		7, 8, 9
	).det() << std::endl;

	std::cout << Matrix<3, 3, float> (
		1, 0, 0,
		0, 5, 0,
		0, 0, 1
	).det() << std::endl;

	std::cout << MathLib::Matf<3, 3> (
		1, 2, 3,
		4, 5, 6,
		7, 10, 9
	).det() << std::endl;

	std::cout << MathLib::Matf<4, 4> (
		1, 0, 2, -1, 
		3, 0, 0, 5,
		2, 1, 4, -3, 
		1, 0, 5, 0
	).det() << std::endl;

	std::cout << ";;;;;;;;;;;;;;;;;;;;;" << std::endl;

	Matrix<4, 4, float> mat(
		1, 0, 0, 0,
		0, 5, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);

	Vector<4, float> vec(1, 2, 3, 4);

	std::cout << "LLLLLLLLLLLLLLLLLLLL" << std::endl;
	std::cout << mat << std::endl;	
	std::cout << "VVVVVVVVVVVVVVVVVVVV" << std::endl;
	std::cout << vec << std::endl;
	std::cout << ";;;;;;;;;;;;;;;;;;;;;" << std::endl;

	auto res = (mat * vec);

	std::cout << " ??? " << res << std::endl;

	Matrix <2, 2, float> smallMat (1, 2, 3, 4);

	Matrix <4, 4, float> bigMat (smallMat, smallMat, smallMat, smallMat);

	std::cout << bigMat << std::endl;

	Vector <3, float> vecPart (10, 20, 30);

	std::cout << sizeof (vecPart) / sizeof(float) << std::endl;
	std::cout << vecPart.x << std::endl;
	std::cout << vecPart.y << std::endl;
	std::cout << vecPart.z << std::endl;

	std::cout << "sizeof: " << std::endl;
	std::cout << sizeof (Matrix <2, 2, double>) << std::endl;

	std::cout << vecPart(0) << std::endl;
	std::cout << bigMat(1, 1) << std::endl;

	cout << " DDDDDDDDDDDDDDDDDDDDDDDDD " << endl;

	MathLib::Matf<4, 4> gMat(
		1, 0, 2, -1, 
		3, 0, 0, 5,
		2, 1, 4, -3, 
		1, 0, 5, 0
	);

	cout << gMat << endl;
	cout << "Det: " << gMat.det() << endl;

	completePivotGE(gMat);

	cout << gMat << endl;

	return 0;
}