#include <iostream>
#include <cassert>
#include <vector>
#include <limits>
#include <tuple>
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

	auto max_square = [ &grid ]( size_t square_size ) {
			tuple< size_t, size_t, int > max_pos{ 0, 0, numeric_limits< int >::min() };

			for( size_t y = 0 ; y < grid_size - square_size; ++y )
				for( size_t x = 0 ; x < grid_size - square_size; ++x )
				{
					int sum = 0;
					for( size_t i = 0 ; i < square_size; ++i )
						for( size_t j = 0 ; j < square_size; ++j )
							sum += grid[ y + i ][ x + j ];

					if( sum > get< 2 >( max_pos ) )
					{
						get< 0 >( max_pos ) = x;
						get< 1 >( max_pos ) = y;
						get< 2 >( max_pos ) = sum;
					}
				}

			return max_pos;
		};

	// PART 1
	auto max_pos_1 = max_square( 3 );
	cout << "1) largest 3x3 square position for serial number " << serial_number << ": "
		<< get< 0 >( max_pos_1 ) << "," << get< 1 >( max_pos_1 ) << " power " << get< 2 >( max_pos_1 ) << endl; 

	// PART 2

	// for( size_t square_size = 1; square_size <= grid_size - 1; ++square_size )
	// {
		// cout << square_size << "..." << endl;
		// auto max_pos = max_square( square_size );
	// }

	return 0;
}
