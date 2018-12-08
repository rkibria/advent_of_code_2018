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

	using Instruction = pair< char, char >;
	vector< Instruction > instructions;

	ifstream file( argv[ 1 ] );
	if( file.is_open() )
	{
		string line, token;
		while( getline( file, line ) )
		{
			istringstream ifs( line );
			ifs >> token;
			assert( token == "Step" );

			char step_1;
			ifs >> step_1;

			for( int i = 0; i < 5; ++i )
				ifs >> token;

			char step_2;
			ifs >> step_2;

			instructions.emplace_back( Instruction( step_1, step_2 ) );
		}
		file.close();
	}
	else
	{
		cerr << "ERROR: could not open file" << endl;
		return -1;
	}

	cout << "Instructions: " << instructions.size() << endl;

	return 0;
}
