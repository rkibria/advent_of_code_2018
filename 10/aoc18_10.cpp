#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <vector>
#include <sstream>
#include <algorithm>
#include <memory>
using namespace std;

struct Point
{
	int x, y, vx, vy;
};

int main( int argc, char* argv[] )
{
	if( argc < 2 )
	{
		cout << "Usage: " << argv[ 0 ] << " <input file>" << endl;
		return -1;
	}

	vector< unique_ptr< Point > > message;

	ifstream file( argv[ 1 ] );
	if( file.is_open() )
	{
		string line, token;
		while( getline( file, line ) )
		{
			istringstream ifs( line );

			ifs >> token;
			assert( token.substr( 0, 10 ) == "position=<" );

			int x{ 0 };
			if( token.length() > 10 )
			{
				assert( token.back() == ',' );
				string x_str = token.substr( 10 );
				x_str.pop_back();
				x = stoi( x_str );
			}
			else
			{
				ifs >> token;
				assert( token.back() == ',' );
				token.pop_back();
				x = stoi( token );
			}

			int y{ 0 };
			ifs >> token;
			assert( token.length() > 0 );
			assert( token.back() == '>' );
			token.pop_back();
			y = stoi( token );

			int vx{ 0 };
			ifs >> token;
			assert( token.substr( 0, 10 ) == "velocity=<" );
			if( token.length() > 10 )
			{
				assert( token.back() == ',' );
				string vx_str = token.substr( 10 );
				vx_str.pop_back();
				vx = stoi( vx_str );
			}
			else
			{
				ifs >> token;
				assert( token.back() == ',' );
				token.pop_back();
				vx = stoi( token );
			}

			int vy{ 0 };
			ifs >> token;
			assert( token.length() > 0 );
			assert( token.back() == '>' );
			token.pop_back();
			vy = stoi( token );

			message.emplace_back( move( make_unique< Point >( Point{ x, y, vx, vy } ) ) );
		}
		file.close();
	}
	else
	{
		cerr << "ERROR: could not open file" << endl;
		return -1;
	}

	cout << "message points: " << message.size() << endl;

	return 0;
}
