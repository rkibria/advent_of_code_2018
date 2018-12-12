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

			// cout << i << " (" << circle.size() << ") " << (static_cast<double>(i) / circle.size()) << endl;

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
		if( i % 10000 == 0 )
			cout << i << "..." << endl;

		if( cur_m % 23 == 0 )
		{
			scores[ cur_p ] += cur_m;

			const size_t cclkw_distance = 7;

			auto other_m_i_itr = circle.begin();
			size_t other_m_i{ 0 };
			if( cur_i >= cclkw_distance )
			{
				// cout << cur_i << endl;
				other_m_i = cur_i - cclkw_distance;

				// other_m_i_itr = get_iterator( other_m_i );

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
				cout << "@@@" << endl;
				circle.insert( circle.end(), cur_m );
				cur_i = circle.size() - 1;
				cur_i_itr = circle.end();
				--cur_i_itr;
			}
			else
			{
				const size_t two_clockwise = ( one_clockwise + 1 ) % circle.size();

				// cout << "+++" << endl;
				++cur_i_itr;
				if( cur_i_itr == circle.end() )
				{
					cur_i_itr = circle.begin();
					cout << "+++R1 " << two_clockwise << endl;
				}

				++cur_i_itr;
				if( cur_i_itr == circle.end() )
				{
					cur_i_itr = circle.begin();
					++cur_i_itr;
					cout << "+++R2 " << two_clockwise << endl;
				}

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
