#include <iostream>

#include "bitmap.h"
#include "dither.h"
#include <bitset>

// g++ *.cpp *.h -o exename
// ./exename
using namespace std;

int main() {
	// @FIXME Rn it doesn't allow for editing of alpha data

    vector<uint8_t> palette = {0, 0, 0, 255, 0, 0};
    BitMap bmp = BitMap("Input/shaun.bmp");
    Image& img = bmp;
    Dither::apply(img, palette);
    // Dither::apply(img, bmp.quantize(4));
    bmp.save("Output/dithered.bmp");
    
    // BitMap test = BitMap(50, 50);
    // Dither::apply((Image&)test, palette);
    // test.save("Output/test.bmp");
    return 0;
}