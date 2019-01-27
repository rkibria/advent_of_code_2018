#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <deque>
#include <limits>
#include <array>

auto print_vector = [](const auto& v) {
	for(auto& i : v)
		std::clog << i << " ";
	std::clog << std::endl;
};

using IndexVector = std::vector<size_t>;

struct Pos {
	size_t x, y;

	bool operator<(const Pos& b) const {
		return y < b.y || (y == b.y && x < b.x);
	}
};

std::ostream& operator<<(std::ostream& os, const Pos& pos) {
	os << "(" << pos.x << "," << pos.y << ")";
	return os;
}

inline bool operator==(const Pos& lhs, const Pos& rhs) {return lhs.x == rhs.x && lhs.y == rhs.y;}
inline bool operator!=(const Pos& lhs, const Pos& rhs) {return !(lhs == rhs);}

using PosVector = std::vector<Pos>;

auto get_adjc_pos_arr(const Pos& pos) {
	return std::array<Pos, 4>{
		Pos{pos.x, pos.y - 1},
		Pos{pos.x - 1, pos.y},
		Pos{pos.x + 1, pos.y},
		Pos{pos.x, pos.y + 1}
		};
}


enum class Race {elf, goblin};

const int ATTACK_POWER = 3;

const auto C_EMPTY = '.';
const auto C_WALL = '#';
const auto C_ELF = 'E';
const auto C_GOBLIN = 'G';


struct Fighter {
	Pos pos;
	Race race;
	int hp = 200;

	auto alive() const {return hp > 0;}

	void attack() {
		hp -= ATTACK_POWER;
	}

	auto to_char() const {return (race == Race::elf) ? C_ELF : C_GOBLIN;}
	auto to_string() const {
		std::stringstream ss;
		ss << to_char() << "(" << pos << ","
			<< hp << ")"
			<< (alive() ? "*" : "");
		return ss.str();
	}
};

using FighterCntr = std::vector<std::unique_ptr<Fighter>>;

auto sort_fgtr_cntr_read_order = [](auto& fighters) {
	std::sort(fighters.begin(), fighters.end(),
		[](const std::unique_ptr<Fighter>& a,
			const std::unique_ptr<Fighter>& b) {
				return a->pos < b->pos;
			});
};


using ArenaCntr = std::vector<std::string>;

class ArenaMap {
	ArenaCntr cntr;

public:
	ArenaMap() {}

	void add_row(const std::string& row) {
		if(!cntr.empty() && cntr.back().size() != row.size()) {
			throw std::runtime_error("Arena rows must all be equal length");
		}
		cntr.push_back(row);
	}

	auto& get_cntr() {return cntr;}

	auto height() const {return cntr.size();}
	auto width() const {
		assert(!cntr.empty() && !cntr[0].empty());
		return cntr[0].size();
	}

	const auto& 	get(size_t x, size_t y) const {return cntr[y][x];}
	auto& 		get(size_t x, size_t y) {return cntr[y][x];}
	const auto&	get(const Pos& pos) const {return cntr[pos.y][pos.x];}
	auto&		get(const Pos& pos) {return cntr[pos.y][pos.x];}
};


using DistanceType = unsigned int;
using DistanceContainer = std::vector<std::vector<DistanceType>>;

class DistanceMap {
	DistanceContainer cntr;

public:
	DistanceMap() {}

	static const auto DIST_NONE = std::numeric_limits<DistanceType>::max();

	bool 		dist_is_valid(DistanceType dst) const {return dst > 0 && dst != DIST_NONE;}
	Pos 		get_next_step(const Pos& target_pos) const;
	const auto& get(size_t x, size_t y) const {return cntr[y][x];}
	auto& 		get(size_t x, size_t y) {return cntr[y][x];}
	const auto&	get(const Pos& pos) const {return cntr[pos.y][pos.x];}
	auto&		get(const Pos& pos) {return cntr[pos.y][pos.x];}
	void 		resize(size_t width, size_t height);
	void		set_dists_from_start(const Pos& start);
	auto 		to_string() const;
	auto 		valid(size_t x, size_t y) const {return dist_is_valid(get(x, y));}
	auto		valid(const Pos& pos) const {return dist_is_valid(get(pos));}
};

void DistanceMap::resize(size_t width, size_t height) {
	cntr.resize(height);
	for(size_t y = 0; y < height; ++y) {
		cntr[y].resize(width);
	}
}

void DistanceMap::set_dists_from_start(const Pos& start) {
	std::deque<Pos> pos_deq(1, start);

	while(!pos_deq.empty()) {
		const auto pos = pos_deq.front();
		pos_deq.pop_front();

		const auto last_dist = get(pos);
		const auto next_dist = (last_dist == DistanceMap::DIST_NONE) ? 1 : (last_dist + 1);

		for(const auto& adjc_pos : get_adjc_pos_arr(pos)) {
			if(get(adjc_pos) == 0) {
				get(adjc_pos) = next_dist;
				pos_deq.push_back(adjc_pos);
			}
		}
	}
}

auto DistanceMap::to_string() const {
	std::stringstream ss;
	for(const auto& vec : cntr) {
		for(const auto val: vec)
			if(val == DistanceMap::DIST_NONE) {
				ss << "#\t";
			}
			else {
				ss << val << "\t";
			}
		ss << std::endl;
	}
	return ss.str();
}

auto sort_pos_cntr_by_dist = [](auto& cntr, const auto& dist_map) {
	std::sort(cntr.begin(), cntr.end(),
		[&dist_map](const auto& a, const auto& b) {
			const auto dst_a = dist_map.get(a);
			const auto dst_b = dist_map.get(b);
			return dst_a < dst_b || (dst_a == dst_b && a < b);
		});
};

Pos DistanceMap::get_next_step(const Pos& target_pos) const {
	auto cur_pos = target_pos;
	while(true) {
		if(get(cur_pos) == 1) {
			return cur_pos;
		}

		auto adjacs = get_adjc_pos_arr(cur_pos);
		sort_pos_cntr_by_dist(adjacs, *this);
		cur_pos = adjacs[0];
	}
};

class World {
	ArenaMap arena;
	FighterCntr fighters;

	void find_dists(DistanceMap& dist_map, Pos start) const;
	void get_adjc_tgts(IndexVector& adjc_tgts_idx_vec,
		const IndexVector& tgts_idx_vec, const Pos& atkr_pos);
	void get_living_enemies(IndexVector& out_fgtr_idx_vec, size_t fgtr_i);
	void get_reachable_posns_adjc_to_tgts(PosVector& out_pos_vec,
		const IndexVector& tgts_idx_vec, const DistanceMap& dist_map);

public:
	void load(const char*);
	void run();
	auto to_string() const;
};

void World::find_dists(DistanceMap& dist_map, Pos start) const {
	for(size_t y = 0; y < arena.height(); ++y)
		for(size_t x = 0; x < arena.width(); ++x)
			dist_map.get(x, y) = (arena.get(x, y) == C_WALL) ? DistanceMap::DIST_NONE : 0;

	for(const auto& fgtr : fighters)
		if(fgtr->alive())
			dist_map.get(fgtr->pos) = DistanceMap::DIST_NONE;

	dist_map.set_dists_from_start(start);
}

auto World::to_string() const {
	auto arena_copy = arena;
	std::stringstream ss;

	ss << "==== " << fighters.size() << " fighters:" << std::endl;
	for(const auto& fgtr : fighters) {
		if(fgtr->alive())
			arena_copy.get(fgtr->pos) = fgtr->to_char();
		ss << fgtr->to_string() << std::endl;
	}
	for(const auto& s : arena_copy.get_cntr())
		ss << s << std::endl;
	return ss.str();
}

void World::load(const char* input_file) {
	std::ifstream file;
	file.open(input_file);
	if(!file)
		throw std::runtime_error("Could not open input file");

	std::string line;
	while(getline(file, line)) {
		if(line.empty())
			continue;

		std::string arena_row;
		auto current_pos = [ this, &arena_row ]() {
			return Pos{arena_row.size(), arena.height()};
			};

		for(auto c : line) {
			switch(c) {
			case C_EMPTY:
			case C_WALL:
				arena_row.push_back(c);
				break;

			case C_ELF:
				fighters.emplace_back(
					std::make_unique<Fighter>(
						Fighter{current_pos(), Race::elf}));
				arena_row.push_back(C_EMPTY);
				break;

			case C_GOBLIN:
				fighters.emplace_back(
					std::make_unique<Fighter>(
						Fighter{current_pos(), Race::goblin}));
				arena_row.push_back(C_EMPTY);
				break;

			default:
				throw std::runtime_error("Invalid input character");
				break;
			}
		}
		if(arena.height() > 0 && arena.get_cntr().back().size() != arena_row.size()) {
			throw std::runtime_error("Input lines must all be equal length");
		}
		arena.add_row(arena_row);
	}
	file.close();
}

void World::get_living_enemies(IndexVector& out_fgtr_idx_vec, size_t fgtr_i) {
	const auto& fgtr_1 = fighters[fgtr_i];
	assert(fgtr_1->alive());

	out_fgtr_idx_vec.clear();
	for(size_t fgtr_j = 0; fgtr_j < fighters.size(); ++fgtr_j) {
		const auto& fgtr_2 = fighters[fgtr_j];
		if(fgtr_j == fgtr_i
			|| fgtr_1->race == fgtr_2->race
			|| !fgtr_2->alive())
			continue;
		out_fgtr_idx_vec.push_back(fgtr_j);
	}
}

void World::get_reachable_posns_adjc_to_tgts(PosVector& out_pos_vec,
		const IndexVector& tgts_idx_vec, const DistanceMap& dist_map) {
	out_pos_vec.clear();

	for(auto fgtr_i : tgts_idx_vec) {
		const auto& pos = fighters[fgtr_i]->pos;

		for(const auto& adjc_pos : get_adjc_pos_arr(pos))
			if(dist_map.valid(adjc_pos))
				if(std::find(out_pos_vec.begin(), out_pos_vec.end(), adjc_pos) == out_pos_vec.end())
					out_pos_vec.push_back(adjc_pos);
	}
}

void World::get_adjc_tgts(IndexVector& out_adjc_tgts_idx_vec, const IndexVector& tgts_idx_vec, const Pos& atkr_pos) {
	out_adjc_tgts_idx_vec.clear();
	for(const auto& dfnr_i : tgts_idx_vec) {
		const auto& dfnr_pos = fighters[dfnr_i]->pos;

		for(const auto& adjc_pos : get_adjc_pos_arr(atkr_pos)) {
			if(dfnr_pos == adjc_pos) {
				out_adjc_tgts_idx_vec.push_back(dfnr_i);
				break;
			}
		}
	}
}

void World::run() {
	DistanceMap dist_map;
	dist_map.resize(arena.width(), arena.height());

	IndexVector tgts_idx_vec;
	PosVector reachable_pos_vec;
	IndexVector adjc_tgts_idx_vec;

	auto sort_adjc_tgts_by_attack_order = [this, &adjc_tgts_idx_vec]() {
		std::sort(adjc_tgts_idx_vec.begin(), adjc_tgts_idx_vec.end(),
			[this](const auto& a, const auto& b) {
				const auto& fa = fighters[a];
				const auto& fb = fighters[b];
				return (fa->hp < fb->hp) || (fa->hp == fb->hp && fa->pos < fb->pos);
			});
	};

	auto check_for_attack = [this, &sort_adjc_tgts_by_attack_order,
			&adjc_tgts_idx_vec, &tgts_idx_vec](const auto& pos) {
		get_adjc_tgts(adjc_tgts_idx_vec, tgts_idx_vec, pos);
		if(!adjc_tgts_idx_vec.empty()) {
			sort_adjc_tgts_by_attack_order();
			fighters[adjc_tgts_idx_vec.front()]->attack();
		}
		return !adjc_tgts_idx_vec.empty();
	};

	bool done = false;
	int combat_round = 0;
	while(!done) {
		combat_round += 1;
		std::clog << "=== ROUND " << combat_round << std::endl;

		sort_fgtr_cntr_read_order(fighters);
		std::clog << to_string() << std::endl;

		for(size_t atkr_i = 0; atkr_i < fighters.size(); ++atkr_i) {
			auto& atkr = fighters[atkr_i];
			if(!atkr->alive())
				continue;

			get_living_enemies(tgts_idx_vec, atkr_i);
			if(tgts_idx_vec.empty()) {
				std::clog << "NO MORE TARGETS\n";
				done = true;
				break;
			}

			find_dists(dist_map, atkr->pos);

			if(check_for_attack(atkr->pos))
				continue;

			get_reachable_posns_adjc_to_tgts(reachable_pos_vec, tgts_idx_vec, dist_map);
			if(reachable_pos_vec.empty())
				continue;

			sort_pos_cntr_by_dist(reachable_pos_vec, dist_map);
			atkr->pos = dist_map.get_next_step(reachable_pos_vec.front());

			check_for_attack(atkr->pos);
		}
	}

	auto sum_hits = 0;
	for(const auto& fgtr : fighters)
		if(fgtr->hp > 0)
			sum_hits += fgtr->hp;
	std::clog << "sum of hit points: " << sum_hits << std::endl;

	std::cout << sum_hits * (combat_round - 1) << std::endl;
}

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cout << "Usage: " << argv[0] << " <input file>" << std::endl;
		return -1;
	}

	World w;
	w.load(argv[1]);
	w.run();
}
