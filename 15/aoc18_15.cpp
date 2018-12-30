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

struct Fighter {
	Pos pos;
	Race race;
	int hp;
};

using FighterContainer = vector<unique_ptr<Fighter>>;

FighterContainer parse_file(const char* input_file) {
	FighterContainer fighters;

	ifstream file;
	file.open(input_file);
	if(!file)
		throw runtime_error("Could not open input file");

	string line;
	while(getline(file, line)) {
		assert(!line.empty());
		for(auto c : line) {
			switch(c) {
			case '.':
			case '#':
				break;
			case 'E':
				cout << "E" << endl;
				break;
			case 'G':
				cout << "G" << endl;
				break;
			default:
				assert(false);
				break;
			}
		}
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

	return 0;
}

