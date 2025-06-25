#ifndef IMAGE_H
#define IMAGE_H

#include <cstdint>
#include <vector>
#include <algorithm>

// #include "dither.h"

using namespace std;

struct Image {
	virtual void open(const char *filename) = 0;
	virtual void save(const char *filename) = 0;
	virtual int getWidth() = 0;
	virtual int getHeight() = 0;
	virtual bool hasAlpha() = 0;

	uint8_t* getPixel(int r, int c);
	void edit(double contrast, double exposure, double brightness);

	protected:
		vector<uint8_t> pixels;
};
#endif