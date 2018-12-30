#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>
#include <memory>
#include <algorithm>
using namespace std;

using Pos = pair<size_t, size_t>;

enum class Race {Elf, Goblin};

struct Fighter {
	Pos pos;
	Race race;
	int hp;
};


int main(int argc, char* argv[]) {
	if( argc < 2 ) {
		cout << "Usage: " << argv[0] << " <input file>" << endl;
		return -1;
	}

	ifstream file(argv[1]);
	if(file.is_open()) {
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
	}
	else{
		cerr << "ERROR: could not open file" << endl;
		return -1;
	}

	return 0;
}

