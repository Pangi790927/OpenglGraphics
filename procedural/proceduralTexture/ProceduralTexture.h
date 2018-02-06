#ifndef PROCEDURAL_TEXTURE_H
#define PROCEDURAL_TEXTURE_H

#include "Texture.h"
#include "RandGenerator.h"

class ProceduralTexture : public Texture {
public:
	ProceduralTexture (int width = 64, int height = 64, int pixelSize = 3) 
	: Texture(width, height, pixelSize)
	{};

	int seed = 312;

	Point3f getRandColor (int row, int col) {
		namespace RG = RandGenerator;
		RG::srand(row + seed);
		int x = RG::rand();

		RG::srand(col + x + seed);
		int y = RG::rand();
		
		RG::srand(x * y + seed);
		return Point3f(
			float(RG::rand() % RG::MAX_RAND) / float(RG::MAX_RAND),
			float(RG::rand() % RG::MAX_RAND) / float(RG::MAX_RAND),
			float(RG::rand() % RG::MAX_RAND) / float(RG::MAX_RAND)
		);
	}
		
	bool inPicture (int row, int col) {
		return row >= 0 && col >= 0 && row < height && col < height;
	}

	Point3f getColor (int row, int col, int dist = 3) {
		Point3f sum = Point3f(0, 0, 0);
		int count = 0;
		for (int i = -dist; i < dist; i++) {
			for (int j = -dist; j < dist; j++) {
				sum = sum + getRandColor(row + i, col + j);
				count++;
			}
		}

		return sum / double(count);
	}

	Point3f mixColors (int row, int col, int dist = 3) {
		Point3f sum = Point3f(0, 0, 0);
		int count = 0;
		for (int i = -dist; i < dist; i++) {
			for (int j = -dist; j < dist; j++) {
				if (inPicture(row + i, col + j)) {
					sum = sum + get_rgb(row + i, col + j);
					count++;
				}
			}
		}

		return sum / double(count);
	}

	void createRandTexture () {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) { 
				Point3f color = getColor(i, j, 3);
				double val = (color.x + color.y + color.z) / 3.0f;
				color = Point3f(val * 1.15, val * 1.15, val * 0.7);
				set_rgb(i, j, color);
			}
		}

		// for (int i = 0; i < height; i++)
		// 	for (int j = 0; j < width; j++)
		// 		set_rgb(i, j, getColor(i, j, 30));

		// for (int i = 0; i < height; i++)
		// 	for (int j = 0; j < width; j++)
		// 		set_rgb(i, j, Point3f(i < height / 2, j < width / 2, 0));


		// std::vector<std::vector<Point3f>> aux(height, std::vector<Point3f>(width));

		// for (int i = 0; i < height; i++)
		// 	for (int j = 0; j < width; j++)
		// 		aux[i][j] = mixColors(i, j, 7);

		// for (int i = 0; i < height; i++)
		// 	for (int j = 0; j < width; j++)
		// 		set_rgb(i, j, aux[i][j]);

		generateOpenGLTexture();
	}
};

#endif