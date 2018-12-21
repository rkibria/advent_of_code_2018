#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <vector>
#include <sstream>
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
		string line, token;

		getline( file, line ); 
		istringstream ifs( line );

		ifs >> token;
		assert( token == "initial" );

		ifs >> token;
		assert( token == "state:" );

		ifs >> token;

		while( getline( file, line ) )
		{
			if( !line.empty() )
			{
				istringstream ifs( line );
				ifs >> token;

				ifs >> token;
				assert( token == "=>" );

				ifs >> token;
			}
		}
		file.close();
	}
	else
	{
		cerr << "ERROR: could not open file" << endl;
		return -1;
	}

	return 0;
}

