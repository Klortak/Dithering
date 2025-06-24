#ifndef IMAGE_H
#define IMAGE_H

#include <cstdint>
#include <vector>
// #include <algorithm>

struct Image {
	virtual void open(const char *filename) = 0;
	virtual void save(const char *filename) = 0;
	virtual int getWidth() = 0;
	virtual int getHeight() = 0;
	virtual uint8_t* getPixel(int r, int c) = 0;

	void edit(double contrast, double exposure, double brightness) {
		for(int row{0}; row < this->getHeight(); row++) {
			for(int col{0}; col < this->getWidth(); col++) {
				uint8_t* pixel{this->getPixel(row, col)};

				uint8_t& r{pixel[2]};
				uint8_t& g{pixel[1]};
				uint8_t& b{pixel[0]};

				int cR{(int)std::max(0.0, std::min((r - 127.5) * contrast + 127.5, 255.0))};
				int cG{(int)std::max(0.0, std::min((g - 127.5) * contrast + 127.5, 255.0))};
				int cB{(int)std::max(0.0, std::min((b - 127.5) * contrast + 127.5, 255.0))};

				int eR{(int)std::max(0.0, std::min(cR * exposure, 255.0))};
				int eG{(int)std::max(0.0, std::min(cG * exposure, 255.0))};
				int eB{(int)std::max(0.0, std::min(cB * exposure, 255.0))};
				
				int bR{(int)std::max(0.0, std::min(eR + brightness, 255.0))};
				int bG{(int)std::max(0.0, std::min(eG + brightness, 255.0))};
				int bB{(int)std::max(0.0, std::min(eB + brightness, 255.0))};

				r = bR;
				g = bG;
				b = bB;
			}
		}
	}

	void quantize(int count) {
		if(this->getWidth() * this->getHeight() > 1099511627775) {
			throw length_error("Jesus christ why is your image so big");
		}
		
		vector<uint64_t> occurences;
		for(int row{0}; row < this->getHeight(); row++) {
			for(int col{0}; col < this->getWidth(); col++) {
				uint8_t* pixel{this->getPixel(row, col)};
				uint32_t formattedRGB{(pixel[0] << 16) | (pixel[1] << 8) | pixel[2]}; // RRRRRRRRGGGGGGGGBBBBBBBB
				uint64_t val{formattedRGB << 40}; // Put the rgb values at the beginning of the number

				uint32_t keyMask{0xFFFFFF << 40}; // 24 bit mask
				int entryInd = -1;
				for(int i = 0; i < occurences.size(); i++) {
					if(occurences[i] & keyMask == val) {
						entryInd = i;
						break;
					}
				}

				if(entryInd > -1) {
					// Increment count / it would technically overflow into the rgb values eventually but if you're dithering a photo that is a gigapixel in size you're doing something wrong
					occurences[entryInd]++
				}
				
			}
		}
	}

	protected:
		std::vector<uint8_t> pixels;
};
#endif