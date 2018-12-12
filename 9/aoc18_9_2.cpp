#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <list>
using namespace std;

int main( int argc, char * argv[] )
{
	const size_t num_players = 405;
	const size_t last_marble = 71700 * 100;

	cout << "number of players: " << num_players << endl;
	cout << "last marble: " << last_marble << endl;

	using CircleContainer = list< int >;

	CircleContainer circle( 1 );
	vector< size_t > scores( num_players );

	size_t cur_m = 1;
	size_t cur_i = 0;
	auto cur_i_itr = circle.begin();
	size_t cur_p = 0;

	auto get_iterator = [ &circle ]( size_t i ) {
			assert( i < circle.size() );

			auto itr = circle.begin();

			if( i < circle.size() / 2 )
			{
				for( size_t j = 0; j < i; ++j )
					++itr;
			}
			else
			{
				itr = circle.end();
				--itr;

				const auto ss = circle.size() - i;
				for( size_t j = 0; j < ss; ++j )
					--itr;
			}

			return itr;
		};

	for( size_t i = 0; i < last_marble + 1; ++i )
	{
		if( cur_m % 23 == 0 )
		{
			scores[ cur_p ] += cur_m;

			const size_t cclkw_distance = 7;

			auto other_m_i_itr = circle.begin();
			size_t other_m_i{ 0 };
			if( cur_i >= cclkw_distance )
			{
				other_m_i = cur_i - cclkw_distance;

				other_m_i_itr = cur_i_itr;
				for( size_t k = 0; k < cclkw_distance; ++k )
				{
					assert( other_m_i_itr != circle.begin() );
					--other_m_i_itr;
				}
			}
			else
			{
				other_m_i = circle.size() - ( cclkw_distance - cur_i );
				other_m_i_itr = get_iterator( other_m_i );
			}

			scores[ cur_p ] += *other_m_i_itr;

			cur_i_itr = circle.erase( other_m_i_itr );
			cur_i = other_m_i;
		}
		else
		{
			const size_t one_clockwise = ( cur_i + 1 ) % circle.size();
			if( one_clockwise == circle.size() - 1 )
			{
				circle.insert( circle.end(), cur_m );
				cur_i = circle.size() - 1;
				cur_i_itr = circle.end();
				--cur_i_itr;
			}
			else
			{
				const size_t two_clockwise = ( one_clockwise + 1 ) % circle.size();

				++cur_i_itr;
				if( cur_i_itr == circle.end() )
				{
					cur_i_itr = circle.begin();
				}

				++cur_i_itr;
				if( cur_i_itr == circle.end() )
				{
					cur_i_itr = circle.begin();
					++cur_i_itr;
				}

				cur_i_itr = circle.insert( cur_i_itr, cur_m );
				cur_i = two_clockwise;
			}
		}

		++cur_m;

		cur_p = ( cur_p + 1 ) % num_players;
	}

	cout << "high score: " << *max_element( scores.begin(), scores.end() );

	return 0;
}
