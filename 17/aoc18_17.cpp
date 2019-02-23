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

	void fill_water(size_t x, size_t y);

public:
	Ground(size_t w, size_t h);

	auto get(size_t x, size_t y) const {
		assert(x < width);
		assert(y < height);
		return gnd[y * width + x];
	}

	auto& get(size_t x, size_t y) {
		assert(x < width);
		assert(y < height);
		return gnd[y * width + x];
	}

	void set_vrtcl_clay(size_t x, size_t start_y, size_t end_y);
	void set_hzntl_clay(size_t y, size_t start_x, size_t end_x);

	void run_water(size_t spring_x);

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

void Ground::run_water(size_t spring_x) {
	fill_water(spring_x, 0);
}

void Ground::fill_water(size_t x, size_t y) {
	auto& tile = get(x, y);

	if(tile == GroundKind::clay || tile == GroundKind::water)
		return;

	std::clog << x << "," << y << std::endl;
	assert(tile == GroundKind::sand);

	tile = GroundKind::water;

	if(y < height - 1) {
		fill_water(x, y + 1);

		if(get(x, y + 1) == GroundKind::clay) {
			if(x > 0)
				fill_water(x - 1, y);

			if(x < width - 1)
				fill_water(x + 1, y);
		}
	}

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

	gnd.run_water(4);
	// gnd.run_water(1);

	std::clog << gnd << std::endl;
}
