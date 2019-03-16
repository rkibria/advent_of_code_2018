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
	water = '~',
};

using GroundVector = std::vector<GroundKind>;

class Ground
{
private:
	size_t width, height;
	GroundVector gnd;

	bool run_water_rcrsv(size_t x, size_t y);

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

	void set_vrtcl_clay(size_t x, size_t start_y, size_t end_y);
	void set_hzntl_clay(size_t y, size_t start_x, size_t end_x);

	void run_water(size_t spring_x);

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
	run_water_rcrsv(spring_x, 0);
}

bool Ground::run_water_rcrsv(size_t x, size_t y) {
	if(x >= width || y >= height)
		return true;

	if(get(x, y) != GroundKind::sand)
		return false;

	auto& tile = get(x, y);
	tile = GroundKind::water;

	auto flow_down = [this, x, y]() {return run_water_rcrsv(x, y + 1);};
	auto flow_left = [this, x, y]() {return run_water_rcrsv(x - 1, y);};
	auto flow_right = [this, x, y]() {return run_water_rcrsv(x + 1, y);};

	if(!flow_down()) {
		const auto l_res = flow_left();
		const auto r_res = flow_right();
		return l_res | r_res;
	}
	else
		return true;
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
			token.pop_back();

			scn.is_hzntl = (token[0] == 'y');

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

void part_1(const std::vector<Scan>& scans) {
	auto scans_which_are_hzntl = [&scans](bool is_hzntl) {
		std::vector<Scan> out_vec(scans.size());
		auto it = std::copy_if(scans.begin(), scans.end(), out_vec.begin(),
			[is_hzntl](const Scan& scn) {return scn.is_hzntl == is_hzntl;});
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

	auto hzntl_scans = scans_which_are_hzntl(true);
	auto vrtcl_scans = scans_which_are_hzntl(false);

	const std::pair<size_t, size_t> hzntl_x_range{minimum_start(hzntl_scans), maximum_end(hzntl_scans)};
	const std::pair<size_t, size_t> hzntl_y_range{minimum_coord(hzntl_scans), maximum_coord(hzntl_scans)};

	const std::pair<size_t, size_t> vrtcl_x_range{minimum_coord(vrtcl_scans), maximum_coord(vrtcl_scans)};
	const std::pair<size_t, size_t> vrtcl_y_range{minimum_start(vrtcl_scans), maximum_end(vrtcl_scans)};

	const std::pair<size_t, size_t> x_range{std::min(hzntl_x_range.first, vrtcl_x_range.first),
		std::max(hzntl_x_range.second, vrtcl_x_range.second)};
	const std::pair<size_t, size_t> y_range{std::min(hzntl_y_range.first, vrtcl_y_range.first),
		std::max(hzntl_y_range.second, vrtcl_y_range.second)};
	std::clog << "x range: " << x_range.first << " -> " << x_range.second << std::endl;
	std::clog << "y range: " << y_range.first << " -> " << y_range.second << std::endl;

	const auto gnd_width = x_range.second - x_range.first + 1;
	const auto gnd_height = y_range.second - y_range.first + 1;
	std::clog << "ground area: " << gnd_width << "x" << gnd_height << std::endl;

	Ground gnd(gnd_width, gnd_height);

	for(const auto& scn : scans) {
		if(scn.is_hzntl)
			gnd.set_hzntl_clay(scn.coord - y_range.first, scn.start - x_range.first, scn.end - x_range.first);
		else
			gnd.set_vrtcl_clay(scn.coord - x_range.first, scn.start - y_range.first, scn.end - y_range.first);
	}

	gnd.run_water(500 - x_range.first);

	std::cout << gnd << std::endl;
}

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cout << "Usage: " << argv[0] << " <input file>" << std::endl;
		return -1;
	}

	auto scans = load(argv[1]);
	std::clog << "scans: " << scans.size() << std::endl;

	part_1(scans);
}
