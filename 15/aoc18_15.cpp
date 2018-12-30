#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>
#include <memory>
#include <algorithm>
#include <stdexcept>
using namespace std;

using Pos = pair<size_t, size_t>;

enum class Race {Elf, Goblin};

const int ATTACK_POWER = 3;

struct Fighter {
	Pos pos;
	Race race;
	int hp = 200;
};

using FighterContainer = vector<unique_ptr<Fighter>>;

FighterContainer parse_file(const char* input_file) {
	FighterContainer fighters;

	ifstream file;
	file.open(input_file);
	if(!file)
		throw runtime_error("Could not open input file");

	string line;
	size_t y_pos{0};
	while(getline(file, line)) {
		assert(!line.empty());
		
		size_t x_pos{0};
		for(auto c : line) {
			switch(c) {
			case '.':
			case '#':
				break;
			case 'E':
				fighters.emplace_back(make_unique<Fighter>(Fighter{Pos(x_pos, y_pos), Race::Elf}));
				break;
			case 'G':
				fighters.emplace_back(make_unique<Fighter>(Fighter{Pos(x_pos, y_pos), Race::Goblin}));
				break;
			default:
				assert(false);
				break;
			}
			++x_pos;
		}
		++y_pos;
	}
	file.close();

	return fighters;
}

int main(int argc, char* argv[]) {
	if( argc < 2 ) {
		cout << "Usage: " << argv[0] << " <input file>" << endl;
		return -1;
	}

	FighterContainer fighters = parse_file(argv[1]);
	clog << "fighters: " << fighters.size() << endl;

	return 0;
}

