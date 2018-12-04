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

	bool overlaps( const Claim& c ) const
	{
		if( x > ( c.x + c.w ) || c.x > ( x + w ) )
			return false;

		if( y > ( c.y + c.h ) || c.y > ( y + h ) )
			return false;

		return true;
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

		vector<bool> has_overlaps( claims.size() );
		for( size_t i = 0; i < claims.size(); ++i )
		{
			for( size_t j = i + 1; j < claims.size(); ++j )
			{
				const auto& c1 = claims[ i ];
				const auto& c2 = claims[ j ];
				const auto overlaps = c1.overlaps( c2 );
				if( overlaps )
				{
					has_overlaps[ i ] = true;
					has_overlaps[ j ] = true;
				}
			}
		}

		for( size_t i = 0; i < has_overlaps.size(); ++i )
		{
			if( !has_overlaps[ i ] )
				cout << "claim " << i + 1 << " has no overlaps" << endl;
		}

	}
	else
	{
		cerr << "ERROR: could not open file" << endl;
	}

	return -1;
}
