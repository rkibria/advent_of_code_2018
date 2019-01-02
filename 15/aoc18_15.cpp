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

using Pos = std::pair<size_t, size_t>;

std::ostream& operator<<(std::ostream& os, const Pos& pos) {
	os << "(" << pos.first << "," << pos.second << ")";
	return os;
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
	auto to_char() const {return (race == Race::elf) ? C_ELF : C_GOBLIN;}
	auto to_string() const {
		std::stringstream ss;
		ss << to_char() << "(" << pos.first << "," << pos.second << ": "
			<< hp << ")"
			<< (alive() ? "*" : "");
		return ss.str();
	}
};

using FighterContainer = std::vector<std::unique_ptr<Fighter>>;
using ArenaContainer = std::vector<std::string>;
using DistancesContainer = std::vector<std::vector<int>>;

class World {
	ArenaContainer arena;
	FighterContainer fighters;

	auto arena_height() const {return arena.size();}
	auto arena_width() const {
		assert(!arena.empty() && !arena[0].empty());
		return arena[0].size();
	}

	void find_distances(DistancesContainer& dists, Pos start) const;

public:
	void load(const char*);
	void run();
	auto to_string() const;
};

void World::find_distances(DistancesContainer& dists, Pos start) const {
	dists.resize(arena_height());
	const auto width = arena_width();
	for(size_t row_i = 0; row_i < arena_height(); ++row_i) {
		dists[row_i].resize(width);
		for(size_t col_i = 0; col_i < width; ++col_i)
			dists[row_i][col_i] = (arena[row_i][col_i] == C_WALL) ? -1 : 0;
	}

	for(const auto& fgtr : fighters)
		if(fgtr->alive())
			dists[fgtr->pos.second][fgtr->pos.first] = -1;

	std::deque<Pos> posns(1, start);

	while(!posns.empty()) {
		const auto pos = posns.front();
		posns.pop_front();
		
		const int last_dist = dists[pos.second][pos.first];
		const int next_dist = (last_dist == -1) ? 1 : (last_dist + 1);

		auto inc_and_queue = [&dists, &posns, &next_dist](size_t x, size_t y) {
				if(dists[y][x] == 0) {
					dists[y][x] = next_dist;
					posns.push_back(Pos{x, y});
				}
			};

		inc_and_queue(pos.first, pos.second - 1);
		inc_and_queue(pos.first - 1, pos.second);
		inc_and_queue(pos.first + 1, pos.second);
		inc_and_queue(pos.first, pos.second + 1);
	}
}

auto World::to_string() const {
	auto arn = arena;
	std::stringstream ss;

	ss << "==== " << fighters.size() << " fighters:" << std::endl;
	for(const auto& fgtr : fighters) {
		arn[fgtr->pos.second][fgtr->pos.first] = fgtr->to_char();
		ss << fgtr->to_string() << std::endl;
	}
	for(const auto& s : arn)
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
			return Pos(arena_row.size(), arena.size());
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
		if(!arena.empty() && arena.back().size() != arena_row.size()) {
			throw std::runtime_error("Input lines must all be equal length");
		}
		arena.push_back(arena_row);
	}
	file.close();
}

auto distances_to_string(const DistancesContainer& dists) {
	std::stringstream ss;
	for(const auto& vec : dists) {
		for(const auto val: vec)
			ss << val << "\t";
		ss << std::endl;
	}
	return ss.str();
}

void World::run() {
	auto reading_order_pred = [](const auto& a, const auto& b) {
		return a.second < b.second
			|| (a.second == b.second
				&& a.first < b.first);
	};

	auto sort_fighters_reading_order = [&reading_order_pred, this]() {
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

	auto print_vector = [](const auto& v) {
		for(auto& i : v)
			std::clog << i << " ";
		std::clog << std::endl;
	};

	auto find_reachable = [this](auto& result, const auto& targets, const auto& dists) {
		result.clear();

		auto add_if_free = [&dists, &result](auto pos_x, auto pos_y) {
			if(dists[pos_y][pos_x] > 0) {
				auto pos = Pos{pos_x, pos_y};
				if(std::find(result.begin(), result.end(), pos) == result.end())
					result.push_back(pos);
			}
		};

		for(auto fgtr_i : targets) {
			const auto& pos = fighters[fgtr_i]->pos;
			add_if_free(pos.first, pos.second - 1);
			add_if_free(pos.first - 1, pos.second);
			add_if_free(pos.first + 1, pos.second);
			add_if_free(pos.first, pos.second + 1);
		}
	};

	auto sort_posns_distance = [](auto& vec, const auto& dists) {
		std::sort(vec.begin(), vec.end(),
			[&dists](const auto& a, const auto& b) {
				return dists[a.second][a.first] < dists[b.second][b.first];
			});
	};

	auto find_attackable = [this](auto& attackable, const auto& targets, const auto& atkr_pos) {
		attackable.clear();
		for(const auto& dfnr_i : targets) {
			const auto& dfnr_pos = fighters[dfnr_i]->pos;
			if((dfnr_pos.first == atkr_pos.first
				&& dfnr_pos.second == atkr_pos.second - 1)
				|| (dfnr_pos.first == atkr_pos.first - 1
				&& dfnr_pos.second == atkr_pos.second)
				|| (dfnr_pos.first == atkr_pos.first + 1
				&& dfnr_pos.second == atkr_pos.second)
				|| (dfnr_pos.first == atkr_pos.first
				&& dfnr_pos.second == atkr_pos.second + 1))
				attackable.push_back(dfnr_i);
		}
	};

	DistancesContainer dists;
	std::vector<size_t> targets;
	std::vector<Pos> reachable;
	std::vector<size_t> attackable;

	bool done = false;
	while(!done) {
		sort_fighters_reading_order();
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
				done = true;
				break;
			}

			find_distances(dists, atkr->pos);
			std::clog << distances_to_string(dists);

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

			sort_posns_distance(reachable, dists);
			
			std::clog << "reachable: ";
			print_vector(reachable);

			const auto min_pos = reachable[0];
			const auto min_dist = dists[min_pos.second][min_pos.first];
			reachable.erase(std::remove_if(reachable.begin(), reachable.end(),
				[&min_dist, &dists](const auto& pos) {
					return dists[pos.second][pos.first] > min_dist;
					}
				));
			sort_posns_reading_order(reachable);

			std::clog << "nearest: ";
			print_vector(reachable);

			done = true;
			break;
		}
	}

}

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cout << "Usage: " << argv[0] << " <input file>" << std::endl;
		return -1;
	}

	World wld;
	wld.load(argv[1]);
	wld.run();

	return 0;
}

