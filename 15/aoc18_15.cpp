#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <sstream>

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

class World {
	ArenaContainer arena;
	FighterContainer fighters;

public:
	void load(const char*);
	void sort_fighters();
	auto to_string() const;
};

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

	return 0;
}

