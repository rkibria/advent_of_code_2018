#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cassert>
#include <sstream>
using namespace std;

struct Claim
{
	unsigned int x, y, w, h;

	bool contains_square( unsigned int sx, unsigned int sy ) const
	{
		return sx >= x && sx < x + w && sy >= y && sy < y + h;
	}
};

ostream& operator<<( ostream& os, const Claim& c )
{
	os << "(" << c.x << ", " << c.y << " : " << c.w << "x" << c.h << ")";
	return os;
}

int main( int argc, char* argv[] )
{
	if( argc < 2 )
	{
		cout << "Usage: " << argv[ 0 ] << " <input file>" << endl;
		return -1;
	}

	vector<Claim> claims;

	ifstream file( argv[ 1 ] );
	if( file.is_open() )
	{
		string line;
		while( getline( file, line ) )
		{
			assert( !line.empty() );

			istringstream iss( line );

			string claim;
			iss >> claim;

			string at_sign;
			iss >> at_sign;
			assert( at_sign == "@" );

			string pos;
			iss >> pos;
			assert( !pos.empty() );
			assert( pos.back() == ':' );
			pos.pop_back();
			const auto comma_index = pos.find( "," );
			const string x_str = pos.substr( 0, comma_index );
			const string y_str = pos.substr( comma_index + 1, pos.size() );

			string size;
			iss >> size;
			assert( !size.empty() );
			const auto x_index = size.find( "x" );
			const string w_str = size.substr( 0, x_index );
			const string h_str = size.substr( x_index + 1, size.size() );

			claims.emplace_back( Claim{ stoul( x_str ), stoul( y_str ), stoul( w_str ), stoul( h_str ) } );
		}
		file.close();
		cout << "claims: " << claims.size() << endl;

		unsigned int in_more_than_one_claims = 0;
		for( unsigned int sy = 0; sy < 1000; ++sy )
		{
			for( unsigned int sx = 0; sx < 1000; ++sx )
			{
				unsigned int num_contains = 0;
				for( const auto& cl : claims )
				{
					const auto hit = cl.contains_square( sx, sy );
					if( hit )
					{
						++num_contains;
						if( num_contains > 1 )
						{
							++in_more_than_one_claims;
							break;
						}
					}
				}
			}
		}
		cout << "squares in more than one claim: " << in_more_than_one_claims << endl;
	}
	else
	{
		cerr << "ERROR: could not open file" << endl;
	}

	return -1;
}
