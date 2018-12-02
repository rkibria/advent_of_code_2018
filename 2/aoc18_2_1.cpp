#include <iostream>
#include <fstream>
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
	if( file.is_open() )
	{
		string line;
		unsigned int twos = 0;
		unsigned int threes = 0; 

		while( getline( file, line ) )
		{
			unsigned int counts[ 26 ] = { 0 };
			for( const char c : line )
			{
				const unsigned int index = c - 'a';
				assert( index <= 26 );
				++counts[ index ];
			}

			bool has_two = false;
			bool has_three = false;
			for( const unsigned int x : counts )
			{
				if( x == 2 )
					has_two = true;

				if( x == 3 )
					has_three = true;
			}

			if( has_two )
				++twos;
			if( has_three )
				++threes;
		}
		file.close();

		cout << "twos: " << twos << ", threes: " << threes << " -> checksum: " << ( twos * threes ) << endl;
	}
	else
	{
		cerr << "ERROR: could not open file" << endl;
	}

	return 0;
}
