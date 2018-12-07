#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <vector>
#include <sstream>
using namespace std;

struct Point
{
	int x, y;
};

int main( int argc, char* argv[] )
{
	if( argc < 2 )
	{
		cout << "Usage: " << argv[ 0 ] << " <input file>" << endl;
		return -1;
	}

	vector< Point > coords;

	ifstream file( argv[ 1 ] );
	if( file.is_open() )
	{
		string line, token;
		while( getline( file, line ) )
		{
			istringstream ifs( line );
			ifs >> token;
			assert( token.back() == ',' );

			token.pop_back();
			const int x = stoi( token );
			ifs >> token;
			const int y = stoi( token );
			coords.emplace_back( Point{ x, y } );
		}
		file.close();
	}
	else
	{
		cerr << "ERROR: could not open file" << endl;
		return -1;
	}

	cout << "coordinates: " << coords.size() << endl;

	return 0;
}
