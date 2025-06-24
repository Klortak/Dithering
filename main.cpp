#include <iostream>

#include "bitmap.h"
#include "dither.h"
#include <bitset>

// g++ *.cpp *.h -o exename
// ./exename
using namespace std;

int main() {

	// @FIXME Rn it doesn't allow for editing of alpha data
	// BitMap bit = BitMap("Input/shaun.bmp");

    vector<uint8_t> palette = {0, 0, 0, 255, 0, 0};
    BitMap b = BitMap("Input/shaun.bmp");
    Image& bmp = b;
    // bmp.edit(1, 1, 0);
    // @TODO: Need to change the dither function to their own container, maybe an enum?
    // @TODO: Also need to make the dither functions return a new image rather than modifying
    // Dither::dither(bmp, &Dither::bayer, palette);
    // bmp.quantize(8, &Dither::bayer);
    bmp.save("Output/dithered.bmp");
    
    // BitMap test = BitMap(50, 50);
    // Dither::apply((Image&)test, palette);
    // test.save("Output/test.bmp");
    return 0;
}