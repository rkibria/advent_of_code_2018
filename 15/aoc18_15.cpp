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

	bool alive() const {return hp > 0;}
};

using FighterContainer = std::vector<std::unique_ptr<Fighter>>;
using ArenaContainer = std::vector<std::string>;
using DistancesContainer = std::vector<std::vector<int>>;

class World {
	ArenaContainer arena;
	FighterContainer fighters;

public:
	auto arena_height() const {return arena.size();}
	auto arena_width() const {
		assert(!arena.empty() && !arena[0].empty());
		return arena[0].size();
	}

	void load(const char*);
	void sort_fighters();
	auto to_string() const;
	auto create_distances_map() const;
};

auto World::create_distances_map() const {
	DistancesContainer dists(arena_height());
	const auto width = arena_width();
	for(size_t row_i = 0; row_i < arena_height(); ++row_i) {
		dists[row_i].resize(width);
		for(size_t col_i = 0; col_i < width; ++col_i)
			dists[row_i][col_i] = (arena[row_i][col_i] == C_WALL) ? -1 : 0;
	}
	for(const auto& f : fighters)
		if(f->alive())
			dists[f->pos.second][f->pos.first] = -1;
	return dists;
}

void find_distances(DistancesContainer& dists, Pos start) {
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

void World::sort_fighters() {
	std::sort(fighters.begin(), fighters.end(),
		[](const std::unique_ptr<Fighter>& a,
			const std::unique_ptr<Fighter>& b) {
				return a->pos.second < b->pos.second
				|| (a->pos.second == b->pos.second
					&& a->pos.first < b->pos.first);
			});
}

auto World::to_string() const {
	auto arn = arena;
	std::stringstream ss;

	ss << fighters.size() << " fighters:" << std::endl;
	for(const auto& f : fighters) {
		const auto f_ch = (f->race == Race::elf) ? C_ELF : C_GOBLIN;
		arn[f->pos.second][f->pos.first] = f_ch;
		ss << f_ch << " " << f->pos.first << "," << f->pos.second << std::endl;
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

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cout << "Usage: " << argv[0] << " <input file>" << std::endl;
		return -1;
	}

	World world;
	world.load(argv[1]);
	
	world.sort_fighters();

	std::clog << world.to_string() << std::endl;

	auto dists = world.create_distances_map();
	find_distances(dists, Pos{1,1});

	for(const auto& vec : dists) {
		for(const auto val: vec)
			std::cout << val << "\t";
		std::cout << std::endl;
	}

	return 0;
}

