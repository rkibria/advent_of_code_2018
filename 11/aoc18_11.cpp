#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <limits>
using namespace std;

int main( int argc, char* argv[] )
{
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
			auto lvl = rack_id * static_cast< int >( y );
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

	const int serial_number = 8199;
	for( size_t y = 0 ; y < grid_size; ++y )
		for( size_t x = 0 ; x < grid_size; ++x )
			grid[ y ][ x ] = powerlevel( serial_number, x, y );

	int max_sum = numeric_limits< int >::min();
	pair< size_t, size_t > max_pos{ 0, 0 };
	for( size_t y = 0 ; y < grid_size - 3; ++y )
		for( size_t x = 0 ; x < grid_size - 3; ++x )
		{
			int sum = 0;
			for( size_t i = 0 ; i < 3; ++i )
				for( size_t j = 0 ; j < 3; ++j )
					sum += grid[ y + i ][ x + j ];

			if( sum > max_sum )
			{
				max_sum = sum;
				max_pos.first = x;
				max_pos.second = y;
			}
		}

	cout << "1) largest 3x3 square position for serial number " << serial_number << ": "
		<< max_pos.first << "," << max_pos.second << " (total power " << max_sum << ")" << endl; 

	return 0;
}
