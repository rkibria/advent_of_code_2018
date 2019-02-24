#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>
#include <sstream>

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

	assert(tile == GroundKind::sand);
	tile = GroundKind::water;

	auto flow_down = [this, x, y]() {fill_water(x, y + 1);};
	auto flow_up = [this, x, y]() {fill_water(x, y - 1);};
	auto flow_left = [this, x, y]() {fill_water(x - 1, y);};
	auto flow_right = [this, x, y]() {fill_water(x + 1, y);};

	if(y < height - 1) {
		const auto below = get(x, y + 1);
		if(below == GroundKind::clay || below == GroundKind::water) {
			if(x > 0) {
				const auto left = get(x - 1, y);
				if(left == GroundKind::clay && y > 0)
					flow_up();
				else
					flow_left();
			}

			if(x < width - 1) {
				const auto right = get(x + 1, y);
				if(right == GroundKind::clay && y > 0)
					flow_up();
				else
					flow_right();
			}
		}
		else {
			flow_down();
		}
	}
}

struct Scan {
	bool is_hzntl;
	size_t coord;
	size_t start;
	size_t end;
};

auto load(const char* filename) {
	std::vector<Scan> scans;
	std::ifstream file(filename);
	if(file.is_open())
	{
		std::string line;
		while(getline(file, line))
		{
			if(line.empty())
				continue;

			Scan scn;

			std::istringstream iss(line);

			std::string token;
			iss >> token;

			scans.emplace_back(scn);
		}
	}
	return scans;
}

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cout << "Usage: " << argv[0] << " <input file>" << std::endl;
		return -1;
	}

	const auto scans = load(argv[1]);
	std::clog << "scans: " << scans.size() << std::endl;

	Ground gnd(10, 10);

	gnd.set_vrtcl_clay(2, 3, 7);
	gnd.set_vrtcl_clay(6, 3, 7);
	gnd.set_hzntl_clay(7, 2, 6);
	gnd.run_water(4);

	std::clog << gnd << std::endl;
}
