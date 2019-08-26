#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>
#include <sstream>
#include <algorithm>

using GroundType = char;

enum class GroundKind : GroundType {
	none = ' ',
	sand = '.',
	clay = '#',
	flowing_water = '|',
	stagnant_water = '~',
};

using GroundVector = std::vector<GroundKind>;

class Ground
{
private:
	size_t width, height;
	GroundVector gnd;

	bool run_water_recurse(size_t x, size_t y);

public:
	Ground(size_t w, size_t h);

	auto get(size_t x, size_t y) const {
		if(x < width && y < height)
			return gnd[y * width + x];
		else
			return GroundKind::none;
	}

	auto& get(size_t x, size_t y) {
		assert(x < width);
		assert(y < height);
		return gnd[y * width + x];
	}

	void set_ver_clay(size_t x, size_t start_y, size_t end_y);
	void set_hor_clay(size_t y, size_t start_x, size_t end_x);

	void run_water(size_t spring_x);

	auto count_water() const {
		return std::count(gnd.begin(), gnd.end(), GroundKind::stagnant_water)
			+ std::count(gnd.begin(), gnd.end(), GroundKind::flowing_water);
	}

	auto count_stagnant_water() const {
		return std::count(gnd.begin(), gnd.end(), GroundKind::stagnant_water);
	}

	friend std::ostream& operator<<(std::ostream& os, const Ground& gnd);
};

std::ostream& operator<<(std::ostream& os, const Ground& gnd) {
	os << "Ground(" << gnd.width << "x" << gnd.height << ")" << std::endl;
	for(size_t y = 0; y < gnd.height; ++y) {
		for(size_t x = 0; x < gnd.width; ++x) {
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

void Ground::set_ver_clay(size_t x, size_t start_y, size_t end_y) {
	assert(start_y < end_y);
	for(auto y = start_y; y <= end_y; ++y)
		get(x, y) = GroundKind::clay;
}

void Ground::set_hor_clay(size_t y, size_t start_x, size_t end_x) {
	assert(start_x < end_x);
	for(auto x = start_x; x <= end_x; ++x)
		get(x, y) = GroundKind::clay;
}

void Ground::run_water(size_t spring_x) {
	run_water_recurse(spring_x, 0);
}

static size_t recursive_call_count = 0;

bool Ground::run_water_recurse(size_t x, size_t y) {
	// std::cout << *this << std::endl;
	++recursive_call_count;
	if(recursive_call_count % 1000000 == 0) std::clog << recursive_call_count << std::endl; // took about 1 billion calls

	if(x >= width || y >= height)
		return true;

	using Pos = std::pair<size_t, size_t>;
	std::vector<Pos> to_fill;

	to_fill.push_back(Pos{x, y});

	const auto flow_sideways = [this, &to_fill](auto x, auto y, auto go_left) {
		bool flow = false;
		while(x > 0) {
			go_left ? --x : ++x;
			if(get(x, y) != GroundKind::clay) {
				to_fill.push_back(Pos{x, y});
				const auto tile_below = get(x, y + 1);
				if(y < height - 1 && tile_below != GroundKind::clay && tile_below != GroundKind::stagnant_water) {
					flow = run_water_recurse(x, y + 1);
				}
				if(flow) {
					break;
				}
			}
			else {
				flow = false;
				break;
			}
		}
		return flow;
	};

	bool flow = true;
	if(y < height - 1) {
		const auto tile_below = get(x, y + 1);
		if(tile_below != GroundKind::clay && tile_below != GroundKind::stagnant_water) {
			flow = run_water_recurse(x, y + 1);
		}
		else {
			flow = false;
		}

		if(!flow) {
			if(x > 0) {
				const auto left_flow = flow_sideways(x, y, true);
				const auto right_flow = flow_sideways(x, y, false);
				flow = left_flow || right_flow;
			}
		}
	}

	for(const auto& pos : to_fill) {
		get(pos.first, pos.second) = flow ? GroundKind::flowing_water : GroundKind::stagnant_water;
	}

	return flow;
}

struct Scan {
	bool is_hor;
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
			token.pop_back();

			scn.is_hor = (token[0] == 'y');

			token = token.substr(2);
			scn.coord = stoi(token);

			iss >> token;
			token = token.substr(2);
			const auto dots = token.find("..");
			assert(dots != std::string::npos);
			scn.start = stoi(token.substr(0, dots));
			scn.end = stoi(token.substr(dots + 2));

			scans.emplace_back(scn);
		}
	}
	return scans;
}

void solve(const std::vector<Scan>& scans) {
	auto get_scans = [&scans](bool is_hor) {
		std::vector<Scan> out_vec(scans.size());
		auto it = std::copy_if(scans.begin(), scans.end(), out_vec.begin(),
			[is_hor](const Scan& scn) {return scn.is_hor == is_hor;});
		out_vec.resize(std::distance(out_vec.begin(), it));
		return out_vec;
	};

	auto minimum_coord = [](const auto& vec_scans) {
		return (*std::min_element(vec_scans.begin(), vec_scans.end(),
			[](const Scan& a, const Scan& b) {return a.coord < b.coord;})).coord;
	};

	auto maximum_coord = [](const auto& vec_scans) {
		return (*std::max_element(vec_scans.begin(), vec_scans.end(),
			[](const Scan& a, const Scan& b) {return a.coord < b.coord;})).coord;
	};

	auto minimum_start = [](const auto& vec_scans) {
		return (*std::min_element(vec_scans.begin(), vec_scans.end(),
			[](const Scan& a, const Scan& b) {return a.start < b.start;})).start;
	};

	auto maximum_end = [](const auto& vec_scans) {
		return (*std::max_element(vec_scans.begin(), vec_scans.end(),
			[](const Scan& a, const Scan& b) {return a.end < b.end;})).end;
	};

	auto hor_scans = get_scans(true);
	auto ver_scans = get_scans(false);

	const std::pair<size_t, size_t> hor_x_range{minimum_start(hor_scans), maximum_end(hor_scans)};
	const std::pair<size_t, size_t> hor_y_range{minimum_coord(hor_scans), maximum_coord(hor_scans)};

	const std::pair<size_t, size_t> ver_x_range{minimum_coord(ver_scans), maximum_coord(ver_scans)};
	const std::pair<size_t, size_t> ver_y_range{minimum_start(ver_scans), maximum_end(ver_scans)};

	const std::pair<size_t, size_t> x_range{std::min(hor_x_range.first, ver_x_range.first),
		std::max(hor_x_range.second, ver_x_range.second)};
	const std::pair<size_t, size_t> y_range{std::min(hor_y_range.first, ver_y_range.first),
		std::max(hor_y_range.second, ver_y_range.second)};
	std::clog << "x range: " << x_range.first << " -> " << x_range.second << std::endl;
	std::clog << "y range: " << y_range.first << " -> " << y_range.second << std::endl;

	const auto gnd_width = x_range.second - x_range.first + 1;
	const auto gnd_height = y_range.second - y_range.first + 1;
	std::clog << "ground area: " << gnd_width << "x" << gnd_height << std::endl;

	const size_t x_pad = 2;

	Ground gnd(gnd_width + 2 * x_pad, gnd_height);

	for(const auto& scn : scans) {
		if(scn.is_hor)
			gnd.set_hor_clay(scn.coord - y_range.first, scn.start - x_range.first + x_pad, scn.end - x_range.first + x_pad);
		else
			gnd.set_ver_clay(scn.coord - x_range.first + x_pad, scn.start - y_range.first, scn.end - y_range.first);
	}

	gnd.run_water(500 - x_range.first + x_pad);

	std::clog << gnd << std::endl;

	std::cout << gnd.count_water() << std::endl;
	std::cout << gnd.count_stagnant_water() << std::endl;
}

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cout << "Usage: " << argv[0] << " <input file>" << std::endl;
		return -1;
	}

	auto scans = load(argv[1]);
	std::clog << "scans: " << scans.size() << std::endl;

	solve(scans);

	std::clog << "recursive call count: " << recursive_call_count << std::endl;
}
