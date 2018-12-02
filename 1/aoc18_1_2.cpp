#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
using namespace std;

int main( int argc, char * argv[] )
{
	if( argc < 2 )
	{
		cout << "Usage: " << argv[0] << " <input file>" << endl;
		return -1;
	}

	vector<int> inputs;

	ifstream file( argv[1] );
	if( file.is_open() )
	{
		string line;

		while( getline( file, line ) )
		{
			int in = stoi( line );
			inputs.emplace_back( in );
		}

		file.close();
	}

	cout << "inputs: " << inputs.size() << endl;

	set<int> seen;
	int freq = 0;
	while( true )
	{
		for( size_t i = 0; i < inputs.size(); ++i )
		{
			freq += inputs[ i ];
			if( seen.find( freq ) != seen.end() )
			{
				cout << "frequency reached twice: " << freq << endl;
				return 0;
			}
			else
			{
				seen.insert( freq );
			}
		}

		cout << "repeating inputs ..." << endl;
	}

}
