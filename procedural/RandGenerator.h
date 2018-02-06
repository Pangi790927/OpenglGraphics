#ifndef RAND_GENERATOR_H
#define RAND_GENERATOR_H

/// code below from stackoverflow, no idea how the numbers were chosen
namespace RandGenerator
{
	unsigned int g_seed = 0;
	const int MAX_RAND = 32767;

	// Used to seed the generator.           
	inline void srand(int seed) {
	    g_seed = seed;
	}

	// Compute a pseudorandom integer.
	// Output value in range [0, 32767]
	inline int rand(void) {
	    g_seed = (214013 * g_seed + 2531011);
	    return (g_seed >> 16) & 0x7FFF;
	}

	inline double randNorm() {
		return rand() / double(MAX_RAND);
	}
}

#endif