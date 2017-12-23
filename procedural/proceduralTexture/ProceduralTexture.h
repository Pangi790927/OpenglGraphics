#ifndef PROCEDURAL_TEXTURE_H
#define PROCEDURAL_TEXTURE_H

#include "Texture.h"

class ProceduralTexture : public Texture {
public:
	ProceduralTexture (int width = 64, int height = 64, int pixelSize = 3) 
	: Texture(width, height, pixelSize)
	{};

	unsigned int g_seed;

	// Used to seed the generator.           
	inline void fast_srand(int seed) {
	    g_seed = seed;
	}

	// Compute a pseudorandom integer.
	// Output value in range [0, 32767]
	inline int fast_rand(void) {
	    g_seed = (214013*g_seed+2531011);
	    return (g_seed>>16)&0x7FFF;
	}

	Point3f getRandColor (int row, int col) {
		fast_srand(row);
		int x = rand();

		fast_srand(col + x);
		int y = rand();
		
		fast_srand(x * y);
		return Point3f(
			float(32767) / float(fast_rand() % 32767),
			float(32767) / float(fast_rand() % 32767),
			float(32767) / float(fast_rand() % 32767)
		);
	}
	
	Point3f getColor (int row, int col, int dist = 5) {
		Point3f sum = Point3f(0, 0, 0);
		for (int i = -dist; i < dist; i++)
			for (int j = -dist; j < dist; j++)
				sum = sum + getRandColor(row + i, col + j);

		return sum / (dist * dist * 4);
	}

	void createRandTexture() {
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				set_rgb(i, j, getColor(i, j));
			}
		}
		generateOpenGLTexture();
	}
};

#endif