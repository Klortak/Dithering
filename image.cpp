#include <cstdint>
#include <vector>
#include "image.h"

using namespace std;

uint8_t* Image::getPixel(int row, int col) {
    return &(this->pixels[(row * this->getWidth() + col) * (3 + (int)this->hasAlpha())]);
}

void Image::edit(double contrast, double exposure, double brightness) {
    for(int row{0}; row < this->getHeight(); row++) {
        for(int col{0}; col < this->getWidth(); col++) {
            uint8_t* pixel{this->getPixel(row, col)};

            uint8_t& r{pixel[2]};
            uint8_t& g{pixel[1]};
            uint8_t& b{pixel[0]};

            int cR{(int)max(0.0, min((r - 127.5) * contrast + 127.5, 255.0))};
            int cG{(int)max(0.0, min((g - 127.5) * contrast + 127.5, 255.0))};
            int cB{(int)max(0.0, min((b - 127.5) * contrast + 127.5, 255.0))};

            int eR{(int)max(0.0, min(cR * exposure, 255.0))};
            int eG{(int)max(0.0, min(cG * exposure, 255.0))};
            int eB{(int)max(0.0, min(cB * exposure, 255.0))};
            
            int bR{(int)max(0.0, min(eR + brightness, 255.0))};
            int bG{(int)max(0.0, min(eG + brightness, 255.0))};
            int bB{(int)max(0.0, min(eB + brightness, 255.0))};

            r = bR;
            g = bG;
            b = bB;
        }
    }
}