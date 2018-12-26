#include <iostream>
#include <cassert>
#include <list>
using namespace std;

int main( int argc, char* argv[] ) {
	list< int > recipes;

	auto elf_1 = recipes.emplace( recipes.end(), 3 );
	auto elf_2 = recipes.emplace( recipes.end(), 7 );
	
	auto print = [ &recipes, &elf_1, &elf_2 ]() {
			for( auto i = recipes.begin(); i != recipes.end(); ++i ) {
				if( i == elf_1 ) {
					cout << "(" << *i << ") ";
				}
				else if( i == elf_2 ) {
					cout << "[" << *i << "] ";
				}
				else {
					cout << *i << " ";
				}
			}
			cout << endl;
		};

	auto create = [ &recipes, &elf_1, &elf_2 ]() {
			const auto sum = *elf_1 + *elf_2;
			if( sum > 9 ) {
				recipes.push_back( 1 );
			}
			recipes.push_back( sum % 10 );
		};

	print();
	create();
	print();

}

