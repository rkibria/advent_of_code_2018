#include <iostream>
#include <vector>
#include <cassert>
using namespace std;

int main( int argc, char * argv[] )
{
	using CircleContainer = vector< int >;

	CircleContainer circle;

	int cur_m = 0;
	int cur_i = 0;

	for( int i = 0; i < 25; ++i )
	{
		if( circle.empty() )
		{
			circle.insert( circle.begin(), cur_m );
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

		for( size_t j = 0; j < circle.size(); ++j )
		{
			if( j == cur_i )
				cout << ".";
			cout << circle[ j ] << " ";
		}
		cout << endl << endl;
	}

	return 0;
}
