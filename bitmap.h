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
	void open(const char* filename) override {
		ifstream file(filename, ios_base::binary);
		if(!file) {
			throw runtime_error("No file found");
		}

		// READ FILE HEADER
		file.read((char*)&this->header.file, sizeof(BitMapFileHeader)); // Why convert to char* ?
		if(this->header.file.fileType != 0x4D42) {
			throw runtime_error("Unknown filetype");
		}

		// READ INFO HEADER
		file.read((char*)&this->header.info, sizeof(BitMapInfoHeader));
		if(this->header.info.bitCount == 32) { // The file says it is set to alpha mode
			if(this->header.info.size >= (sizeof(BitMapInfoHeader) + sizeof(BitMapColourHeader))) { // If the info header states that there is more data
				// READ COLOUR HEADER
				file.read((char*)&this->header.colour, sizeof(BitMapColourHeader));
				// @TODO check if the colour header is set up correctly
			}
			else {
				throw runtime_error("File is formatted incorrectly");
			}
		}

		// I think bitmaps read from bottom left?
		if(this->getHeight() < 0) { // Why do bitmaps allow negative height???
			throw runtime_error("Im too lazy to implement negative height rn");
		}

		file.seekg(this->header.file.dataOffset, file.beg); // Go to beginning of pixel data
		this->pixels.resize(this->getWidth() * this->getHeight() * (this->header.info.bitCount / 8));

		if(this->getWidth() % 4 == 0) { // If no padding needed
			file.read((char*)this->pixels.data(), this->pixels.size());
		}
		else {
			this->determineStridePadding();
			vector<uint8_t> padding(this->paddedStride - this->rowStride); // Create a vector of size equal to the amount of padding needed
			for(int row{0}; row < this->getHeight(); row++) { // Need to read row by row so the padding can be removed
				file.read((char*)(this->pixels.data() + this->rowStride * row), this->rowStride);
				file.read((char*)padding.data(), padding.size()); // Read in the trash data
			}
		}

		// Convert from BGR to RGB
		for(int row{0}; row < this->getHeight(); row++) {
			for(int col{0}; col < this->getWidth(); col++) {
				uint8_t* pixel = this->getPixel(row, col);
				uint8_t temp = pixel[2];
				pixel[2] = pixel[0];
				pixel[0] = temp;
			}
		}

		file.close();
	}

	void save(const char* filename) override {
		ofstream file(filename, ios_base::binary);
		if(!file) {
			throw runtime_error("File could not be found or created");
		}
		
		this->createHeaders();
		if(this->header.info.bitCount != 24 && this->header.info.bitCount != 32) {
			// throw runtime_error("Incompatible bit count");
		}
		
		file.write((const char*)&this->header.file, sizeof(BitMapFileHeader));
		file.write((const char*)&this->header.info, sizeof(BitMapInfoHeader));

		if(this->header.info.bitCount == 32) {
			file.write((const char*)&this->header.colour, sizeof(BitMapColourHeader));
		}

		// Convert from RGB to BGR
		for(int row{0}; row < this->getHeight(); row++) {
			for(int col{0}; col < this->getWidth(); col++) {
				uint8_t* pixel = this->getPixel(row, col);
				uint8_t temp = pixel[2];
				pixel[2] = pixel[0];
				pixel[0] = temp;
			}
		}
		
		if(this->getWidth() % 4 == 0) { // If no padding is needed
			file.write((const char*)this->pixels.data(), this->pixels.size());
		}
		else {
			this->determineStridePadding();
			const vector<uint8_t> padding(this->paddedStride - this->rowStride); // Create a vector of size equal to the amount of padding needed
			for(int row{0}; row < this->getHeight(); row++) {
				file.write((const char*)(this->pixels.data() + this->rowStride * row), this->rowStride);
				file.write((const char*)padding.data(), padding.size());
			}
		}

		file.close();
	}

	int getWidth() override {
		return this->header.info.width;	
	}

	int getHeight() override {
		return this->header.info.height;
	}

	uint8_t* getPixel(int row, int col) {
		// multiply by bitCount / 8 to account for each colour band being stored
		// add 1 if alpha is included so that way it will still return starting at the colours
		return &(this->pixels[(row * this->getWidth() + col) * (this->header.info.bitCount / 8) + (int)(this->header.info.bitCount == 32)]);
	}

	BitMap(const char* filename) {
		this->open(filename);
	}

	BitMap(int width, int height, bool useAlpha = false) {
		this->header.info.width = width;
		this->header.info.height = height;
		
		if(useAlpha) {
			this->header.info.bitCount = 32;
		}
		else {
			this->header.info.bitCount = 24;
		}
		
		this->pixels = vector<uint8_t>(width * height * (this->header.info.bitCount / 8));
		// Bitmaps require rows to be divisible by 4 bytes
		// this->determineStridePadding();
	}

	private:
		BitMapHeader header; // Apparently you don't need to initialise it
		// vector<uint8_t> pixels; // I think I can change this to be a pointer like in the previous version?

		int rowStride{0};
		int paddedStride{0};

		void createHeaders() {
			this->header.info.size = sizeof(BitMapInfoHeader);
			if(this->header.info.bitCount == 32) { // If alpha info header needs to include the size of colour header
				this->header.info.size += sizeof(BitMapColourHeader);
			}
			
			this->header.file.dataOffset = sizeof(BitMapFileHeader) + this->header.info.size; // Data starts after all headers
			this->header.file.fileSize = this->header.file.dataOffset + this->pixels.size();
			if(this->getWidth() % 4 != 0) { // Add pixel padding to file size
				this->header.file.fileSize += (this->paddedStride - this->rowStride) * this->getHeight();
			}
		}

		void determineStridePadding() {
			this->rowStride = this->getWidth() * (this->header.info.bitCount / 8);
			this->paddedStride = this->rowStride;
			while(this->paddedStride % 4 != 0) {
				this->paddedStride++; // Add onto the byte count until divisible by four
			}
		}
};

#endif