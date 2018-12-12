#include <iostream>
#include <vector>
#include <cassert>
using namespace std;

int main( int argc, char * argv[] )
{
	const size_t num_players = 9;
	const size_t last_marble = 24;

	using CircleContainer = vector< int >;

	CircleContainer circle( 1 );
	vector< size_t > scores( num_players );

	size_t cur_m = 1;
	size_t cur_i = 0;
	size_t cur_p = 0;

	for( int i = 0; i < last_marble + 1; ++i )
	{
		const size_t one_clockwise = ( cur_i + 1 ) % circle.size();
		if( one_clockwise == circle.size() - 1 )
		{
			circle.insert( circle.end(), cur_m );
			cur_i = circle.size() - 1;
		}
		else
		{
			const size_t two_clockwise = ( one_clockwise + 1 ) % circle.size();
			circle.insert( circle.begin() + two_clockwise, cur_m );
			cur_i = two_clockwise;
		}

		++cur_m;

		cout << cur_p + 1 << ": ";
		for( size_t j = 0; j < circle.size(); ++j )
		{
			if( j == cur_i )
				cout << ".";
			cout << circle[ j ] << " ";
		}
		cout << endl << endl;

		cur_p = ( cur_p + 1 ) % num_players;
	}

	return 0;
}
