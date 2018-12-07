#include <cstddef>
#include <iostream>
#include <fstream>
#include <ctype.h>
#include <string>
#include <cassert>
#include <algorithm>
using namespace std;

string collapse_polymer( const string& original )
{
	string polymer = original;
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
		}
	}

	return result;
}

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

	string result_1 = collapse_polymer( polymer );
	cout << "1) polymer length after reaction: " << result_1.size() << endl;

	size_t lowest_length = 0xffffffff;
	char best_unit = 0;
	for( char unit = 'a'; unit <= 'z'; ++unit )
	{
		string p = polymer;
		p.erase( remove_if( p.begin(), p.end(), [ unit ]( char x ) { return x == unit || x == toupper( unit ); } ),
			p.end() );
		string r = collapse_polymer( p );
		cout << "remove unit " << unit << ": polymer length " << r.size() << endl;
		if( r.size() < lowest_length )
		{
			lowest_length = r.size();
			best_unit = unit;
		}
	}
	cout << "2) polymer lowest length " << lowest_length << " when removing unit type " << best_unit << endl;

	return 0;
}
