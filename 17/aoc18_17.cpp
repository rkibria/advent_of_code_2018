#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>

using GroundType = char;

enum class GroundKind : GroundType {
	sand = '.',
	clay = '#',
	water = '~',
};

using GroundVector = std::vector<GroundKind>;

class Ground
{
private:
	size_t width, height;
	GroundVector gnd;

public:
	Ground(size_t w, size_t h);

	auto get(size_t x, size_t y) const {return gnd[y * width + x];}
	auto& get(size_t x, size_t y) {return gnd[y * width + x];}

	void set_vrtcl_clay(size_t x, size_t start_y, size_t end_y);
	void set_hzntl_clay(size_t y, size_t start_x, size_t end_x);

	friend std::ostream& operator<<(std::ostream& os, const Ground& gnd);
};

std::ostream& operator<<(std::ostream& os, const Ground& gnd) {
	os << "Ground(" << gnd.width << "x" << gnd.height << ")" << std::endl;
	for(size_t y = 0; y < gnd.height; ++y) {
		for(size_t x = 0; x < gnd.height; ++x) {
			os << static_cast<char>(gnd.get(x, y));
		}
		os << std::endl;
	}

	return os;
}

Ground::Ground(size_t w, size_t h)
	: width(w),
	height(h),
	gnd(w * h, GroundKind::sand) {
	assert(w > 0 && h > 0);
}

void Ground::set_vrtcl_clay(size_t x, size_t start_y, size_t end_y) {
	assert(start_y < end_y);
	for(auto y = start_y; y <= end_y; ++y)
		get(x, y) = GroundKind::clay;
}

void Ground::set_hzntl_clay(size_t y, size_t start_x, size_t end_x) {
	assert(start_x < end_x);
	for(auto x = start_x; x <= end_x; ++x)
		get(x, y) = GroundKind::clay;
}

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cout << "Usage: " << argv[0] << " <input file>" << std::endl;
		return -1;
	}

	Ground gnd(10, 10);
	gnd.set_vrtcl_clay(2, 3, 7);
	gnd.set_vrtcl_clay(6, 3, 7);
	gnd.set_hzntl_clay(7, 2, 6);
	std::clog << gnd << std::endl;

}
