#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cassert>
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
		vector<string> ids;
		string line;

		while( getline( file, line ) )
		{
			assert( !line.empty() );
			ids.emplace_back( line );
		}
		file.close();

		for( size_t i = 0; i < ids.size(); ++i )
		{
			for( size_t j = i + 1; j < ids.size(); ++j )
			{
				const string& id_1 = ids[ i ];
				const string& id_2 = ids[ j ];
				assert( id_1.size() == id_2.size() );

				unsigned int diffs = 0;
				for( size_t k = 0; k < id_1.size(); ++k )
				{
					if( id_1[ k ] != id_2[ k ] )
						++diffs;
				}

				if( diffs == 1 )
				{
					cout << "result: " << endl << id_1 << endl << id_2 << endl << "->" << endl;
					for( size_t k = 0; k < id_1.size(); ++k )
					{
						if( id_1[ k ] == id_2[ k ] )
							cout << id_1[ k ];
					}
					cout << endl;
					return 0;
				}
			}
		}
	}
	else
	{
		cerr << "ERROR: could not open file" << endl;
	}

	return -1;
}
