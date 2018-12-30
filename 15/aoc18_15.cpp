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

struct World {
	ArenaContainer arena;
	FighterContainer fighters;

	auto to_string() const;
};

auto World::to_string() const {
	auto arn = arena;
	for(const auto& f : fighters) {
		arn[f->pos.second][f->pos.first] = (f->race == Race::elf) ? C_ELF : C_GOBLIN;
	}
	std::stringstream ss;
	for(const auto& s : arn)
		ss << s << std::endl;
	return ss.str();
}

auto parse_file(const char* input_file) {
	World world;

	std::ifstream file;
	file.open(input_file);
	if(!file)
		throw std::runtime_error("Could not open input file");

	std::string line;
	while(getline(file, line)) {
		if(line.empty())
			continue;
		
		std::string arena_row;
		auto current_pos = [ &world, &arena_row ]() {
			return Pos(arena_row.size(), world.arena.size());
			};

		for(auto c : line) {
			switch(c) {
			case C_EMPTY:
			case C_WALL:
				arena_row.push_back(c);
				break;
			case C_ELF:
				world.fighters.emplace_back(
					std::make_unique<Fighter>(
						Fighter{current_pos(), Race::elf}));
				arena_row.push_back(C_EMPTY);
			break;
			case C_GOBLIN:
				world.fighters.emplace_back(
					std::make_unique<Fighter>(
						Fighter{current_pos(), Race::goblin}));
				arena_row.push_back(C_EMPTY);
				break;
			default:
				throw std::runtime_error("Read invalid input character");
				break;
			}
		}
		world.arena.push_back(arena_row);
	}
	file.close();

	return world;
}

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cout << "Usage: " << argv[0] << " <input file>\n";
		return -1;
	}

	auto world = parse_file(argv[1]);

	std::clog << world.to_string() << std::endl;

	std::clog << "fighters: " << world.fighters.size() << std::endl;

	std::sort(world.fighters.begin(), world.fighters.end(),
		[](const std::unique_ptr<Fighter>& a,
			const std::unique_ptr<Fighter>& b) {
				return a->pos.second < b->pos.second
				|| (a->pos.second == b->pos.second
					&& a->pos.first < b->pos.first);
			});

	for(const auto& f : world.fighters) {
		std::cout << f->pos.first << "," << f->pos.second << std::endl;
	}
	return 0;
}

