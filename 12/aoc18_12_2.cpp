#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <vector>
#include <sstream>
#include <map>
#include <time.h>
using namespace std;

int main( int argc, char* argv[] )
{
	if( argc < 2 )
	{
		cout << "Usage: " << argv[ 0 ] << " <input file>" << endl;
		return -1;
	}

	const size_t padding_size = 100;
	const size_t note_size = 5;
	string state( padding_size, '.' );

	map< string, char > notes;

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
		state.append( token );

		while( getline( file, line ) )
		{
			if( !line.empty() )
			{
				string note;
				istringstream ifs( line );
				ifs >> note;
				assert( note.size() == note_size );

				ifs >> token;
				assert( token == "=>" );

				ifs >> token;
				assert( token.length() == 1 );

				notes[ note ] = token[ 0 ];
			}
		}
		file.close();
	}
	else
	{
		cerr << "ERROR: could not open file" << endl;
		return -1;
	}

	state.append( padding_size, '.' );
	cout << "notes: " << notes.size() << endl;

	// PART 1
	string next_state;
	auto grow = [ &state, &next_state, &notes, note_size ]() {	
			next_state = state;
			for( size_t i = 0; i < state.length() - note_size; ++i )
			{
				const string stretch = state.substr( i, note_size );
				if( notes.find( stretch ) != notes.end() )
				{
					next_state[ i + ( note_size / 2 ) ] = notes[ stretch ];
				}
			}
			state.swap( next_state );
		};

	// original: 2 sec
	for( size_t i = 0; i < 100000; ++i )
	{
		grow();
	}
	
	return 0;
}

