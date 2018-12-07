#include <cstddef>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cassert>
#include <sstream>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <array>
#include <numeric>
using namespace std;

int main( int argc, char* argv[] )
{
	if( argc < 2 )
	{
		cout << "Usage: " << argv[ 0 ] << " <input file>" << endl;
		return -1;
	}

	ifstream file( argv[ 1 ] );
	string polymer;
	if( file.is_open() )
	{
		getline( file, polymer );
		assert( !polymer.empty() );
	}
	else
	{
		cerr << "ERROR: could not open file" << endl;
		return -1;
	}

	cout << "polymer length: " << polymer.size() << endl;

	return 0;
}
