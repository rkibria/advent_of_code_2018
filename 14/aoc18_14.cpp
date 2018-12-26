#include <iostream>
#include <cassert>
#include <list>
using namespace std;

int main( int argc, char* argv[] ) {
	list< int > recipes;
	
	auto elf_1 = recipes.end();
	auto elf_2 = recipes.end();

	auto init = [ &recipes, &elf_1, &elf_2 ]() {
			recipes.clear();
			elf_1 = recipes.emplace( recipes.end(), 3 );
			elf_2 = recipes.emplace( recipes.end(), 7 );
		};

	auto print = [ &recipes, &elf_1, &elf_2 ]() {
			for( auto i = recipes.begin(); i != recipes.end(); ++i ) {
				cout << ( ( i == elf_1 ) ? "(" : "" );
				cout << ( ( i == elf_2 ) ? "[" : "" );
				cout << *i;
				cout << ( ( i == elf_2 ) ? "]" : "" );
				cout << ( ( i == elf_1 ) ? ")" : "" );
				cout << " \n";
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

	const size_t num_recipes = 4801; // 864801;

/*	init();
	while( true ) {
		create();
		advance();
		
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
*/
	init();
	while( true ) {
		create();
		advance();

		//print();

		auto remaining = static_cast< int >( num_recipes );
		auto itr = recipes.cend();
		--itr;
		while( remaining > 0 ) {
			const auto digit = remaining % 10;

			if( digit != *itr ) {
				remaining = num_recipes;
				break;
			}

			if( itr == recipes.cbegin() )
				break;
			
			remaining /= 10;
			--itr;
		}

		if( remaining == 0 ) {
			print();
			recipes.erase( ++itr, recipes.cend() );
			cout << "2) " << recipes.size() << endl;
			break;
		}

	}

}

