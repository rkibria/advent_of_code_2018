#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <vector>
#include <sstream>
#include <algorithm>
#include <memory>
#include <limits>
#include <set>
using namespace std;

int main( int argc, char* argv[] )
{
	const int serial_number = 8;

	using RowVector = vector < int >;
	vector< RowVector > grid;

	const size_t grid_size = 300;
	for( size_t i = 0 ; i < grid_size; ++i )
		grid.emplace_back( RowVector( grid_size ) );

	auto hundreds_digit = []( int a ) {
			return ( a / 100 ) % 10;
		};

	auto powerlevel = [ &hundreds_digit ]( int sn, size_t x, size_t y ) {
			const auto rack_id = static_cast< int >( x ) + 10;
			int lvl = rack_id * static_cast< int >( y );
			lvl += sn;
			lvl *= rack_id;
			lvl = hundreds_digit( lvl );
			lvl -= 5;
			return lvl;
		};

	assert( powerlevel( 8, 3, 5 ) == 4 );
	assert( powerlevel( 57, 122, 79 ) == -5 );
	assert( powerlevel( 39, 217, 196 ) == 0 );
	assert( powerlevel( 71, 101, 153 ) == 4 );

	for( size_t y = 0 ; y < grid_size; ++y )
		for( size_t x = 0 ; x < grid_size; ++x )
			grid[ y ][ x ] = powerlevel( serial_number, x, y );

	return 0;
}
