#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>
#include <memory>
#include <algorithm>
#include <set>
using namespace std;

using Pos = pair< size_t, size_t >;
using TracksVector = vector< string >;

enum class Dir { u, d, l, r };
enum class Turn { l, s, r };

struct Cart
{
	size_t x, y;
	Turn t;
	Dir d;

	Pos next_pos() const
	{
		size_t nx, ny;
		switch( d )
		{
		case Dir::u:
			assert( y > 0 );
			nx = x;
			ny = y - 1;
			break;
		case Dir::d:
			nx = x;
			ny = y + 1;
			break;
		case Dir::l:
			assert( x > 0 );
			nx = x - 1;
			ny = y;
			break;
		case Dir::r:
			nx = x + 1;
			ny = y;
			break;

		default:
			break;
		}
		return Pos( nx, ny );
	}

	pair< Dir, Turn > next_direction( char moved_to ) const
	{
		Dir nxt_d = d;
		Turn nxt_t = t;

		switch( moved_to )
		{
		case '/' :
			switch( d )
			{
			case Dir::u : nxt_d = Dir::r; break;
			case Dir::d : nxt_d = Dir::l; break;
			case Dir::l : nxt_d = Dir::d; break;
			case Dir::r : nxt_d = Dir::u; break;
			default: break;
			}
			break;

		case '\\' :
			switch( d )
			{
			case Dir::u : nxt_d = Dir::l; break;
			case Dir::d : nxt_d = Dir::r; break;
			case Dir::l : nxt_d = Dir::u; break;
			case Dir::r : nxt_d = Dir::d; break;
			default: break;
			}
		break;

		case '+' :
			switch( t )
			{
			case Turn::l :
				switch( d )
				{
				case Dir::u : nxt_d = Dir::l;
				case Dir::d : nxt_d = Dir::r;
				case Dir::l : nxt_d = Dir::d;
				case Dir::r : nxt_d = Dir::u;
				default: break;
				}
				nxt_t = Turn::s;
				break;
			case Turn::s :
				nxt_t = Turn::r;
				break;
			case Turn::r :
				switch( d )
				{
				case Dir::u : nxt_d = Dir::r;
				case Dir::d : nxt_d = Dir::l;
				case Dir::l : nxt_d = Dir::u;
				case Dir::r : nxt_d = Dir::d;
				default: break;
				}
				nxt_t = Turn::l;
				break;
			default:
				break;
			}
			break;

		default:
			break;
		}

		return pair< Dir, Turn >( nxt_d, nxt_t );
	}

	Pos move( const TracksVector& tracks )
	{
		Pos nxt_p = next_pos();
		x = nxt_p.first;
		y = nxt_p.second;

		assert( y < tracks.size() );
		assert( x < tracks[ y ].size() );

		const pair< Dir, Turn > nxt_d = next_direction( tracks[ y ][ x ] );
		d = nxt_d.first;
		t = nxt_d.second;
		
		return nxt_p;
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
					carts.emplace_back( make_unique< Cart >( Cart{ x, y, Turn::l, Dir::u } ) );
					line[ x ] = '|';
					break;
				case 'v':
					carts.emplace_back( make_unique< Cart >( Cart{ x, y, Turn::l, Dir::d } ) );
					line[ x ] = '|';
					break;
				case '<':
					carts.emplace_back( make_unique< Cart >( Cart{ x, y, Turn::l, Dir::l } ) );
					line[ x ] = '-';
					break;
				case '>':
					carts.emplace_back( make_unique< Cart >( Cart{ x, y, Turn::l, Dir::r } ) );
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

	set< Pos > cart_posns;
	for( size_t i = 0; i < carts.size(); ++i )
	{
		auto& crt = carts[ i ];

		Pos nxt = crt->move( tracks );

		cart_posns.insert( nxt );

		cout << nxt.first << "," << nxt.second << endl;
	}

	return 0;
}

