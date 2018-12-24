#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>
using namespace std;


enum class Dtn { u, d, l, r };

struct Cart
{
	size_t x, y;
	Dtn d;
};


int main( int argc, char* argv[] )
{
	if( argc < 2 )
	{
		cout << "Usage: " << argv[ 0 ] << " <input file>" << endl;
		return -1;
	}

	vector< string > tracks;
	vector< Cart > carts;

	ifstream file( argv[ 1 ] );
	if( file.is_open() )
	{
		string line;
		while( getline( file, line ) )
		{
			assert( !line.empty() );

			const size_t y = tracks.size();
			for( size_t x = 0; x < line.size(); ++x )
			{
				const char c = line[ x ];
				switch( c )
				{
				case '^':
					carts.emplace_back( Cart{ x, y, Dtn::u } );
					line[ x ] = '|';
					break;
				case 'v':
					carts.emplace_back( Cart{ x, y, Dtn::d } );
					line[ x ] = '|';
					break;
				case '<':
					carts.emplace_back( Cart{ x, y, Dtn::l } );
					line[ x ] = '-';
					break;
				case '>':
					carts.emplace_back( Cart{ x, y, Dtn::r } );
					line[ x ] = '-';
					break;
				default:
					break;
				}
			}

			tracks.emplace_back( line );
		}
		file.close();
	}
	else
	{
		cerr << "ERROR: could not open file" << endl;
		return -1;
	}

	for( auto& s : tracks ) cout << s << endl;

	cout << "carts: " << carts.size() << endl;
	
	return 0;
}

