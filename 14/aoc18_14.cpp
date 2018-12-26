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
				cout << ( ( i == elf_1 ) ? "(" : "" );
				cout << ( ( i == elf_2 ) ? "[" : "" );
				cout << *i;
				cout << ( ( i == elf_2 ) ? "]" : "" );
				cout << ( ( i == elf_1 ) ? ")" : "" );
				cout << " ";
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
	
	auto advance = [ &recipes ]( auto itr, size_t steps ) {
			for( size_t i = 0; i < steps; ++i ) {
				++itr;
				if( itr == recipes.end() ) {
					itr = recipes.begin();
				}
			}
			return itr;
		};
		
	print();

	create();
	elf_1 = advance( elf_1, 1 + *elf_1 );
	elf_2 = advance( elf_2, 1 + *elf_2 );
	print();

}

