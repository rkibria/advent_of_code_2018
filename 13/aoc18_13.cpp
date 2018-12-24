#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>
#include <memory>
#include <algorithm>
#include <set>
using namespace std;

using Coords = pair< size_t, size_t >;
using TracksVector = vector< string >;

enum class Dtn { u, d, l, r };

struct Cart
{
	size_t x, y, turn;
	Dtn d;

	Coords next_pos() const
	{
		size_t nx, ny;
		switch( d )
		{
		case Dtn::u:
			assert( y > 0 );
			nx = x;
			ny = y - 1;
			break;
		case Dtn::d:
			nx = x;
			ny = y + 1;
			break;
		case Dtn::l:
			assert( x > 0 );
			nx = x - 1;
			ny = y;
			break;
		case Dtn::r:
			nx = x + 1;
			ny = y;
			break;

		default:
			break;
		}
		return Coords( nx, ny );
	}

	Coords move( const TracksVector& tracks )
	{
		Coords nxt = next_pos();
		return nxt;
	}

};


int main( int argc, char* argv[] )
{
	if( argc < 2 )
	{
		cout << "Usage: " << argv[ 0 ] << " <input file>" << endl;
		return -1;
	}

	TracksVector tracks;
	vector< unique_ptr< Cart > > carts;

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
					carts.emplace_back( make_unique< Cart >( Cart{ x, y, 0, Dtn::u } ) );
					line[ x ] = '|';
					break;
				case 'v':
					carts.emplace_back( make_unique< Cart >( Cart{ x, y, 0, Dtn::d } ) );
					line[ x ] = '|';
					break;
				case '<':
					carts.emplace_back( make_unique< Cart >( Cart{ x, y, 0, Dtn::l } ) );
					line[ x ] = '-';
					break;
				case '>':
					carts.emplace_back( make_unique< Cart >( Cart{ x, y, 0, Dtn::r } ) );
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

	sort( carts.begin(), carts.end(),
		[]( const unique_ptr< Cart >& a, const unique_ptr< Cart >& b ) { return a->y < b->y; } );

	set< Coords > cart_coords;
	for( size_t i = 0; i < carts.size(); ++i )
	{
		auto& crt = carts[ i ];

		Coords nxt = crt->move( tracks );

		cart_coords.insert( nxt );

		cout << crt->x << "," << crt->y << " to " << nxt.first << "," << nxt.second << endl;
	}

	return 0;
}

