#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main( int argc, char * argv[] )
{
	if( argc < 2 )
	{
		cout << "Usage: " << argv[0] << " <input file>" << endl;
		return -1;
	}

	ifstream file( argv[1] );
	if( file.is_open() )
	{
		string line;
		int freq = 0;

		while( getline( file, line ) )
		{
			int in = stoi( line );
			freq += in;
		}
		cout << "frequency: " << freq << endl;

		file.close();
	}

	return 0;
}
