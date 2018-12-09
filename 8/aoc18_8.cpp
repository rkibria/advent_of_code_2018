#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include <sstream>
using namespace std;

int main( int argc, char * argv[] )
{
	if( argc < 2 )
	{
		cout << "Usage: " << argv[0] << " <input file>" << endl;
		return -1;
	}

	vector< int > input;

	ifstream file( argv[1] );
	if( file.is_open() )
	{
		string line;
		assert( getline( file, line ) );

		istringstream iss( line );
		string token;
		while( !iss.eof() )
		{
			iss >> token;
			input.emplace_back( stoi( token) );
		}

		file.close();
	}

	cout << "input length: " << input.size() << endl;

	return 0;
}
