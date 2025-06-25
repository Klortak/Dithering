#ifndef BITMAP_H
#define BITMAP_H

#include <iostream>
#include <vector>
#include <fstream>
#include "image.h"

using namespace std;

#pragma pack(push, 1) // Supposed to keep the compiler from adding padding
struct BitMapFileHeader {
	uint16_t fileType{0x4D42};	// Filetype is "BM" which is equal to 0x4D42
	uint32_t fileSize{0};  		// Size of the file (in bytes)
	uint16_t reservedA{0};    	// Reserved, always 0
	uint16_t reservedB{0};     	// Reserved, always 0
	uint32_t dataOffset{0};		// Where the pixel data starts (in bytes)
};

struct BitMapInfoHeader {
	uint32_t size{0};				// Size of info header (in bytes)
	int32_t width{0};				// Width of bitmap in pixels
	int32_t height{0};				// Height of bitmap in pixels
	uint16_t planes{1};				// No. of planes for the target device, this is always 1
	uint16_t bitCount{0};			// No. of bits per pixel
	uint32_t compression{0};		// 0 or 3 - uncompressed. THIS PROGRAM CONSIDERS ONLY UNCOMPRESSED BMP images
	uint32_t imageSize{0};			// 0 - for uncompressed images
	int32_t pixelsPerMeterX{0};
	int32_t pixelsPerMeterY{0};
	uint32_t usedColours{0};		// No. color indexes in the color table. Use 0 for the max number of colors allowed by bit_count
	uint32_t importantColours{0};	// No. of colors used for displaying the bitmap. If 0 all colors are required
};

struct BitMapColourHeader {
	uint32_t redMask{0x00ff0000};			// Bit mask for the red channel
	uint32_t greenMask{0x0000ff00};			// Bit mask for the green channel
	uint32_t blueMask{0x000000ff};			// Bit mask for the blue channel
	uint32_t alphaMask{0xff000000};			// Bit mask for the alpha channel
	uint32_t colorSpaceType{0x73524742}; 	// Default "sRGB" (0x73524742)
	uint32_t unused[16]{0};
};

struct BitMapHeader {
	BitMapFileHeader file;
	BitMapInfoHeader info;
	BitMapColourHeader colour;
};

#pragma pack(pop)

struct BitMap : Image {
	void open(const char* filename) override;
	void save(const char* filename) override;

	int getWidth() override;
	int getHeight() override;

	bool hasAlpha() override;

	BitMap(const char* filename);
	BitMap(int width, int height, bool useAlpha = false);

	private:
		BitMapHeader header; // Apparently you don't need to initialise it

		int rowStride{0};
		int paddedStride{0};

		void createHeaders();
		void determineStridePadding();
};

#endif