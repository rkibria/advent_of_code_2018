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
	
	auto advance_elf = [ &recipes ]( auto itr, size_t steps ) {
			for( size_t i = 0; i < steps; ++i ) {
				++itr;
				if( itr == recipes.end() ) {
					itr = recipes.begin();
				}
			}
			return itr;
		};
	
	auto advance = [ &recipes, &elf_1, &elf_2, &advance_elf ]() {
			elf_1 = advance_elf( elf_1, 1 + *elf_1 );
			elf_2 = advance_elf( elf_2, 1 + *elf_2 );
		};

	const size_t num_recipes = 9;

	print();

	for( size_t i = 0; i < 1000; ++i ) {
		create();
		advance();
		print();
		
		if( num_recipes + 10 <= recipes.size() ) {
			cout << "1) scores of ten recipes after " << num_recipes << " recipes: ";
			auto itr = recipes.end();
			for( size_t j = 0; j < 10; ++j ) {
				--itr;
			}
			for( size_t j = 0; j < 10; ++j ) {
				cout << *( itr++ );
			}
			cout << endl;

			break;
		}
	}

}

