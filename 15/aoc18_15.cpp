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


struct Pos {
	size_t x, y;
};

std::ostream& operator<<(std::ostream& os, const Pos& pos) {
	os << "(" << pos.x << "," << pos.y << ")";
	return os;
}

inline bool operator==(const Pos& lhs, const Pos& rhs) {return lhs.x == rhs.x && lhs.y == rhs.y;}
inline bool operator!=(const Pos& lhs, const Pos& rhs) {return !(lhs == rhs);}


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

const auto DIST_NONE = std::numeric_limits<DistanceType>::max();

bool dist_valid(DistanceType dst) {
	return dst > 0 && dst != DIST_NONE;
}

class DistanceMap {
	DistanceContainer cntr;
public:
	DistanceMap() {}
	auto& 		get_cntr() {return cntr;}
	const auto& 	get(size_t x, size_t y) const {return cntr[y][x];}
	auto& 		get(size_t x, size_t y) {return cntr[y][x];}
	const auto&	get(const Pos& pos) const {return cntr[pos.y][pos.x];}
	auto&		get(const Pos& pos) {return cntr[pos.y][pos.x];}
	auto 		to_string() const;
	auto 		valid(size_t x, size_t y) const {return dist_valid(get(x, y));}
	auto		valid(const Pos& pos) const {return dist_valid(get(pos));}
};

auto DistanceMap::to_string() const {
	std::stringstream ss;
	for(const auto& vec : cntr) {
		for(const auto val: vec)
			if(val == DIST_NONE) {
				ss << "#\t";
			}
			else {
				ss << val << "\t";
			}
		ss << std::endl;
	}
	return ss.str();
}


class World {
	ArenaMap arena;
	FighterCntr fighters;

	void find_dists(DistanceMap& dists, Pos start) const;
public:
	void load(const char*);
	void run();
	auto to_string() const;
};

void World::find_dists(DistanceMap& dists, Pos start) const {
	// Mark all walls as blocked, else init with 0
	dists.get_cntr().resize(arena.height());
	const auto width = arena.width();
	for(size_t y = 0; y < arena.height(); ++y) {
		dists.get_cntr()[y].resize(width);
		for(size_t x = 0; x < width; ++x)
			dists.get(x, y) = (arena.get(x, y) == C_WALL) ? DIST_NONE : 0;
	}

	// Mark all fighter's positions as blocked
	for(const auto& fgtr : fighters)
		if(fgtr->alive())
			dists.get(fgtr->pos) = DIST_NONE;

	// Find distances from start position
	std::deque<Pos> posns(1, start);

	while(!posns.empty()) {
		const auto pos = posns.front();
		posns.pop_front();
		
		const auto last_dist = dists.get(pos);
		const auto next_dist = (last_dist == DIST_NONE) ? 1 : (last_dist + 1);

		auto inc_and_queue = [&dists, &posns, &next_dist](size_t x, size_t y) {
				if(dists.get(x, y) == 0) {
					dists.get(x, y) = next_dist;
					posns.push_back(Pos{x, y});
				}
			};

		inc_and_queue(pos.x, pos.y - 1);
		inc_and_queue(pos.x - 1, pos.y);
		inc_and_queue(pos.x + 1, pos.y);
		inc_and_queue(pos.x, pos.y + 1);
	}
}

auto World::to_string() const {
	auto arena_copy = arena;
	std::stringstream ss;

	ss << "==== " << fighters.size() << " fighters:" << std::endl;
	for(const auto& fgtr : fighters) {
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

void World::run() {
	auto reading_order_pred = [](const auto& a, const auto& b) {
		return a.y < b.y || (a.y == b.y	&& a.x < b.x);
	};

	auto sort_fighters = [&reading_order_pred, this]() {
		std::sort(fighters.begin(), fighters.end(),
			[&reading_order_pred](const std::unique_ptr<Fighter>& a,
				const std::unique_ptr<Fighter>& b) {
					return reading_order_pred(a->pos, b->pos);
				});
	};

	auto sort_posns_reading_order = [&reading_order_pred](auto& posns) {
		std::sort(posns.begin(), posns.end(), reading_order_pred);
	};

	auto find_targets = [this](auto& result, auto fgtr_i) {
		const auto& fgtr_1 = fighters[fgtr_i];
		assert(fgtr_1->alive());

		result.clear();
		for(size_t fgtr_j = 0; fgtr_j < fighters.size(); ++fgtr_j) {
			const auto& fgtr_2 = fighters[fgtr_j];
			if(fgtr_j == fgtr_i
				|| fgtr_1->race == fgtr_2->race
				|| !fgtr_2->alive())
				continue;
			result.push_back(fgtr_j);
		}
	};

	auto find_reachable = [this](auto& result, const auto& targets, const auto& dists) {
		result.clear();

		auto add_if_free = [&dists, &result](auto x, auto y) {
			if(dists.valid(x, y)) {
				Pos pos{x, y};
				if(std::find(result.begin(), result.end(), pos) == result.end())
					result.push_back(pos);
			}
		};

		for(auto fgtr_i : targets) {
			const auto& pos = fighters[fgtr_i]->pos;
			add_if_free(pos.x, pos.y - 1);
			add_if_free(pos.x - 1, pos.y);
			add_if_free(pos.x + 1, pos.y);
			add_if_free(pos.x, pos.y + 1);
		}
	};

	auto sort_posns_distance = [&reading_order_pred](auto& vec, const auto& dists) {
		std::sort(vec.begin(), vec.end(),
			[&reading_order_pred, &dists](const auto& a, const auto& b) {
				const auto dst_a = dists.get(a);
				const auto dst_b = dists.get(b);
				return dst_a < dst_b
					|| (dst_a == dst_b && reading_order_pred(a, b));
			});
	};

	auto find_attackable = [this](auto& attackable, const auto& targets, const auto& atkr_pos) {
		attackable.clear();
		for(const auto& dfnr_i : targets) {
			const auto& dfnr_pos = fighters[dfnr_i]->pos;
			if((dfnr_pos.x == atkr_pos.x
				&& dfnr_pos.y == atkr_pos.y - 1)
				|| (dfnr_pos.x == atkr_pos.x - 1
				&& dfnr_pos.y == atkr_pos.y)
				|| (dfnr_pos.x == atkr_pos.x + 1
				&& dfnr_pos.y == atkr_pos.y)
				|| (dfnr_pos.x == atkr_pos.x
				&& dfnr_pos.y == atkr_pos.y + 1))
				attackable.push_back(dfnr_i);
		}
	};

	auto find_next_step = [&sort_posns_distance](const auto& target_pos, const auto& dists) {
		auto cur_pos = target_pos;
		std::array<Pos, 4> adjacs;
		while(true) {
			if(dists.get(cur_pos) == 1) {
				return cur_pos;
			}

			adjacs[0] = Pos{cur_pos.x, cur_pos.y - 1};
			adjacs[1] = Pos{cur_pos.x - 1, cur_pos.y};
			adjacs[2] = Pos{cur_pos.x + 1, cur_pos.y};
			adjacs[3] = Pos{cur_pos.x, cur_pos.y + 1};
			sort_posns_distance(adjacs, dists);
			cur_pos = adjacs[0];
		}
	};

	DistanceMap dists;
	std::vector<size_t> targets;
	std::vector<Pos> reachable;
	std::vector<size_t> attackable;

	bool done = false;
	while(!done) {
		sort_fighters();
		std::clog << to_string() << std::endl;

		for(size_t atkr_i = 0; atkr_i < fighters.size(); ++atkr_i) {
			auto& atkr = fighters[atkr_i];
			if(!atkr->alive())
				continue;

			std::clog << "attacking: " << atkr->to_string() << std::endl;

			find_targets(targets, atkr_i);
			std::clog << "viable targets: ";
			print_vector(targets);

			if(targets.empty()) {
				std::clog << "NO MORE TARGETS\n";
				done = true;
				break;
			}

			find_dists(dists, atkr->pos);
			std::clog << dists.to_string();

			find_attackable(attackable, targets, atkr->pos);
			if(!attackable.empty()) {
				std::clog << "attack order: ";
				print_vector(attackable);
				throw std::runtime_error("TODO combat");
				continue;
			}

			// Get vector of positions adjacent to targets that
			// can be reached by attacker
			find_reachable(reachable, targets, dists);

			if(reachable.empty())
				continue;

			sort_posns_distance(reachable, dists);
			
			std::clog << "reachable: ";
			print_vector(reachable);

			const auto min_pos = reachable[0];
			const auto min_dist = dists.get(min_pos);
			reachable.erase(std::remove_if(reachable.begin(), reachable.end(),
				[&min_dist, &dists](const auto& pos) {return dists.get(pos) > min_dist;}));
			sort_posns_reading_order(reachable);

			std::clog << "nearest: ";
			print_vector(reachable);

			const auto target_pos = reachable[0];
			std::clog << "target_pos: " << target_pos << std::endl;

			const auto next_pos = find_next_step(target_pos, dists);
			std::clog << "next: " << next_pos << std::endl;
		
			atkr->pos = next_pos;
		}
	}

}

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cout << "Usage: " << argv[0] << " <input file>" << std::endl;
		return -1;
	}

	World w;
	w.load(argv[1]);
	w.run();

	return 0;
}

