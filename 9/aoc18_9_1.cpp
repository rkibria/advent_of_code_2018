#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>
using namespace std;

int main( int argc, char * argv[] )
{
	const size_t num_players = 405;
	const size_t last_marble = 71700;

	cout << "number of players: " << num_players << endl;
	cout << "last marble: " << last_marble << endl;

	using CircleContainer = vector< int >;

	CircleContainer circle( 1 );
	vector< size_t > scores( num_players );

	size_t cur_m = 1;
	size_t cur_i = 0;
	size_t cur_p = 0;

	for( size_t i = 0; i < last_marble + 1; ++i )
	{
		if( cur_m % 23 == 0)
		{
			scores[ cur_p ] += cur_m;

			size_t other_m_i{ 0 };
			if( cur_i >= 7 )
				other_m_i = cur_i - 7;
			else
			{
				other_m_i = circle.size() - ( 7 - cur_i );
			}

			scores[ cur_p ] += circle[ other_m_i ];
			circle.erase( circle.begin() + other_m_i );
			cur_i = other_m_i;
		}
		else
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
		}

		++cur_m;

		cur_p = ( cur_p + 1 ) % num_players;
	}

	cout << "high score: " << *max_element( scores.begin(), scores.end() );

	return 0;
}
