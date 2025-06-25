#ifndef DITHER_H
#define DITHER_H

#include <vector>
#include "image.h"

struct Dither {
	double bayer(int row, int col);
	static void apply(Image& img, vector<uint8_t> palette); // To make it easy rn we are going to assume that the palette is sorted by brightness

	private:
		// Interleave bits and reverse them at the same time
		// Will take half of finalBitCount from each number and merge them
		// and then reverse that new merged number
		int reverseInterleave(int a, int b, int finalBitCount);
};

#endif