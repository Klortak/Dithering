// #include <iostream>
#include <vector>
#include <fstream>

#include "image.h"
#include "bitmap.h"

using namespace std;

void BitMap::open(const char* filename) {
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

void BitMap::save(const char* filename) {
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

int BitMap::getWidth() {
    return this->header.info.width;	
}

int BitMap::getHeight() {
    return this->header.info.height;
}

bool BitMap::hasAlpha() {
    return this->header.info.bitCount == 32;
}

BitMap::BitMap(const char* filename) {
    this->open(filename);
}

BitMap::BitMap(int width, int height, bool useAlpha) {
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

void BitMap::createHeaders() {
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

void BitMap::determineStridePadding() {
    this->rowStride = this->getWidth() * (this->header.info.bitCount / 8);
    this->paddedStride = this->rowStride;
    while(this->paddedStride % 4 != 0) {
        this->paddedStride++; // Add onto the byte count until divisible by four
    }
}