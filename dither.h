#ifndef DITHER_H
#define DITHER_H
#include "image.h"
#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
using namespace std;

struct Dither {
	double bayer(int row, int col) {
		// I'm keeping it at 16 because it means everything will go up to 255
		const int sideLen{16}; // Needs to be a power of 2
		const int valCount = (sideLen * sideLen);
		// @NOTE: cutting off the bits at 4 works the same as doing mod 16
		// double val{(double)this->reverseBits(this->interleaveBits(col, col ^ row, 4), 8)}; // Get the raw value from the matrix
		double val{(double)this->reverseInterleave(col, col ^ row, 8)};
		val -= ((valCount - 1) / 2.0); // Subtract half the max value to make the sum add up to zero
		val /= valCount; // Scale everything down to be between -.5 and .5
		return val;
	}

	// To make it easy rn we are going to assume that the palette is sorted by brightness
	static void apply(Image& img, vector<uint8_t> palette) {
		Dither d;
		const double colourSpread{255.0 / (palette.size() / 3)}; // Only expect rgb palettes, no rgba
		for(int row{0}; row < img.getHeight(); row++) {
			for(int col{0}; col < img.getWidth(); col++) {
				uint8_t* pixel{img.getPixel(row, col)};
				// Create variables that allow direct modifcation 
				uint8_t& r{pixel[0]};
				uint8_t& g{pixel[1]};
				uint8_t& b{pixel[2]};
		
				double ditherVal{d.bayer(row, col)};
				// Weigh each band and scale the dithering based on amount of colours
				// Keep them between 0 and 255
				double wR{max(0.0, min(r + ditherVal * colourSpread, 255.0))};
				double wG{max(0.0, min(g + ditherVal * colourSpread, 255.0))};
				double wB{max(0.0, min(b + ditherVal * colourSpread, 255.0))};

				// Weigh the bands based on how their brightness is percieved
				double lum{(wR * 3 + wG * 4 + wB) / 8}; // Divide by 8 because the val will be 8 times larger than the 0-255 range
				int ind{(int)((lum / 256) * (palette.size() / 3))};

				r = palette[ind * 3 + 0];
				g = palette[ind * 3 + 1];
				b = palette[ind * 3 + 2];
			}
		}
	}
	private:
		// Interleave bits and reverse them at the same time
		// Will take half of finalBitCount from each number and merge them
		// and then reverse that new merged number
		int reverseInterleave(int a, int b, int finalBitCount) {
			int c{0};
			int depth{finalBitCount / 2};
			for(int i{0}; i < depth; i++) {
				// Get the bit at the end of each number
				int aBit{(a >> i) & 1};
				int bBit{(b >> i) & 1};

				// Add `a` on the right and `b` on the left
				c |= aBit << (2 * (depth - i - 1));
				c |= bBit << (2 * (depth - i - 1) + 1);
			}
			return c;
		}
};

#endif