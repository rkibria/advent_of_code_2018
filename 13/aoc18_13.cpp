#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>
#include <memory>
#include <algorithm>
using namespace std;

using Pos = pair< size_t, size_t >;
using TracksVector = vector< string >;

enum class Dir { u, d, l, r };
enum class Turn { l, s, r };

struct Cart
{
	size_t x, y;
	Dir d;
	Turn t{ Turn::l };
	bool ok{ true };

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
				case Dir::u : nxt_d = Dir::l; break;
				case Dir::d : nxt_d = Dir::r; break;
				case Dir::l : nxt_d = Dir::d; break;
				case Dir::r : nxt_d = Dir::u; break;
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
				case Dir::u : nxt_d = Dir::r; break;
				case Dir::d : nxt_d = Dir::l; break;
				case Dir::l : nxt_d = Dir::u; break;
				case Dir::r : nxt_d = Dir::d; break;
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
		const auto nxt_p = next_pos();
		x = nxt_p.first;
		y = nxt_p.second;

		assert( y < tracks.size() );
		assert( x < tracks[ y ].size() );

		const auto nxt_d = next_direction( tracks[ y ][ x ] );
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
					carts.emplace_back( make_unique< Cart >( Cart{ x, y, Dir::u } ) );
					line[ x ] = '|';
					break;
				case 'v':
					carts.emplace_back( make_unique< Cart >( Cart{ x, y, Dir::d } ) );
					line[ x ] = '|';
					break;
				case '<':
					carts.emplace_back( make_unique< Cart >( Cart{ x, y, Dir::l } ) );
					line[ x ] = '-';
					break;
				case '>':
					carts.emplace_back( make_unique< Cart >( Cart{ x, y, Dir::r } ) );
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

	auto print = [ &tracks, &carts ]() {
			auto tt = tracks;
			for( const auto& c : carts )
			{
				if( !c->ok )
					continue;
				switch( c->d )
				{
				case Dir::u : tt[ c->y ][ c->x ] = '^'; break;
				case Dir::d : tt[ c->y ][ c->x ] = 'v'; break;
				case Dir::l : tt[ c->y ][ c->x ] = '<'; break;
				case Dir::r : tt[ c->y ][ c->x ] = '>'; break;
				default: break;
				}
			}
			for( const auto& s : tt ) cout << s << endl;
			cout << endl;
		};

	auto num_ok = [ &carts ]() {
			return count_if( carts.begin(), carts.end(),
				[]( const auto& c ) { return c->ok; } );
		};

	cout << "carts: " << num_ok() << endl;

	bool done = false;
	while( !done )
	{
		print();

		if( num_ok() <= 1 )
		{
			for( auto& crt : carts )
				if( crt->ok )
				{
					cout << "2) last remaining cart: " 
						<< crt->x << "," << crt->y << endl;
					break;
				}
			done = true;
			break;
		}

		sort( carts.begin(), carts.end(),
			[]( const unique_ptr< Cart >& a, const unique_ptr< Cart >& b )
			{
				return ( a->y < b->y ) || ( a->y == b->y && a->x < b->x );
			});
	
		for( size_t i = 0; i < carts.size(); ++i )
		{
			auto& crt = carts[ i ];
			if( !crt->ok )
				continue;

			const auto nxt = crt->move( tracks );
			
			for( size_t j = 0; j < carts.size(); ++j )
			{
				if( i == j )
					continue;

				auto& crt2 = carts[ j ];
				if( !crt2->ok )
					continue;

				if( crt2->x == nxt.first && crt2->y == nxt.second )
				{
					cout << "1) collision at " << nxt.first << "," << nxt.second << endl;
					crt->ok = false;
					crt2->ok = false;
					break;
				}
			}
		}

	}

	return 0;
}

