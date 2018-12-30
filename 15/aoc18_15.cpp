#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>
#include <memory>
#include <algorithm>
#include <stdexcept>

using Pos = std::pair<size_t, size_t>;

enum class Race {Elf, Goblin};

const int ATTACK_POWER = 3;

struct Fighter {
	Pos pos;
	Race race;
	int hp = 200;
};

using FighterContainer = std::vector<std::unique_ptr<Fighter>>;
using ArenaContainer = std::vector<std::string>;

struct World {
	ArenaContainer arena;
	FighterContainer fighters;
};

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
			case '.':
			case '#':
				arena_row.push_back(c);
				break;
			case 'E':
				world.fighters.emplace_back(
					std::make_unique<Fighter>(
						Fighter{current_pos(), Race::Elf}));
				arena_row.push_back('.');
			break;
			case 'G':
				world.fighters.emplace_back(
					std::make_unique<Fighter>(
						Fighter{current_pos(), Race::Goblin}));
				arena_row.push_back('.');
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
	if( argc < 2 ) {
		std::cout << "Usage: " << argv[0] << " <input file>\n";
		return -1;
	}

	auto world = parse_file(argv[1]);
	std::clog << "fighters: " << world.fighters.size() << "\n";

	return 0;
}

