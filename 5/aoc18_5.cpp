#include <cstddef>
#include <iostream>
#include <fstream>
#include <ctype.h>
#include <string>
#include <cassert>
using namespace std;

int main( int argc, char* argv[] )
{
	if( argc < 2 )
	{
		cout << "Usage: " << argv[ 0 ] << " <input file>" << endl;
		return -1;
	}

	ifstream file( argv[ 1 ] );
	string polymer;
	if( file.is_open() )
	{
		getline( file, polymer );
		assert( !polymer.empty() );
	}
	else
	{
		cerr << "ERROR: could not open file" << endl;
		return -1;
	}

	cout << "polymer length: " << polymer.size() << endl;

	string result;
	result.reserve( polymer.size() );

	bool done = false;
	while( !done )
	{
		size_t i = 0;
		while( i < polymer.size() )
		{
			const auto c = polymer[ i ];

			if( i < polymer.size() - 1 )
			{
				const auto next_c = polymer[ i + 1 ];

				if( tolower( c ) == tolower( next_c ) && c != next_c )
				{
					i += 2;
				}
				else
				{
					result.push_back( c );
					++i;
				}
			}
			else
			{
				result.push_back( c );
				++i;
			}
		}

		if( result.size() == polymer.size() )
		{
			done = true;
		}
		else
		{
			polymer = result;
			result.clear();
			cout << "reacting..." << endl;
		}
	}
	cout << "1) polymer length after reaction: " << result.size() << endl;

	return 0;
}
