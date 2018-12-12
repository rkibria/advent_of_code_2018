#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <list>
using namespace std;

int main( int argc, char * argv[] )
{
	const size_t num_players = 405; // 9; // 10405;
	const size_t last_marble = 71700 * 100; // 25; // 71700; // * 100;

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
			for( size_t j = 0; j < i; ++j )
				++itr;
			return itr;
		};

	for( size_t i = 0; i < last_marble + 1; ++i )
	{
		if( i % 10000 == 0 )
			cout << i << "..." << endl;

		if( cur_m % 23 == 0 )
		{
			scores[ cur_p ] += cur_m;

			size_t other_m_i{ 0 };
			if( cur_i >= 7 )
				other_m_i = cur_i - 7;
			else
			{
				other_m_i = circle.size() - ( 7 - cur_i );
			}

			auto other_m_i_itr = get_iterator( other_m_i );

			scores[ cur_p ] += *other_m_i_itr;

			other_m_i_itr = circle.erase( other_m_i_itr );
			cur_i = other_m_i;
			cur_i_itr = other_m_i_itr;
			// cout << "!!!" << endl;
		}
		else
		{
			const size_t one_clockwise = ( cur_i + 1 ) % circle.size();
			if( one_clockwise == circle.size() - 1 )
			{
				// cout << "@@@" << endl;
				circle.insert( circle.end(), cur_m );
				cur_i = circle.size() - 1;
				cur_i_itr = circle.end();
			}
			else
			{
				// cout << "+++" << endl;
				++cur_i_itr;
				if( cur_i_itr == circle.end() )
				{
					cur_i_itr = circle.begin();
					// cout << "+++R1" << endl;
				}

				++cur_i_itr;
				if( cur_i_itr == circle.end() )
				{
					cur_i_itr = circle.begin();
					// cout << "+++R2" << endl;
				}

				const size_t two_clockwise = ( one_clockwise + 1 ) % circle.size();

				cur_i_itr = circle.insert( cur_i_itr, cur_m );
				cur_i = two_clockwise;
			}
		}

		++cur_m;

		// cout << cur_p + 1 << ": ";
		// auto itr_j = circle.begin();
		// for( size_t j = 0; j < circle.size(); ++j )
		// {
			// if( j == cur_i )
				// cout << ".";
			// cout << *itr_j << " ";
			// itr_j++;
		// }
		// cout << endl << endl;

		cur_p = ( cur_p + 1 ) % num_players;
	}

	cout << "high score: " << *max_element( scores.begin(), scores.end() );

	return 0;
}
