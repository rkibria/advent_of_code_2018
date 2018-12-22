#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <vector>
#include <sstream>
#include <map>
using namespace std;

int main( int argc, char* argv[] )
{
	if( argc < 2 )
	{
		cout << "Usage: " << argv[ 0 ] << " <input file>" << endl;
		return -1;
	}

	const size_t zero_pos = 20;
	const size_t note_size = 5;
	string state( zero_pos, '.' );

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

	state.append( zero_pos, '.' );
	cout << "initial state: " << state << endl;
	cout << "notes: " << notes.size() << endl;

	// PART 1
	for( size_t i = 0; i < state.length() - note_size; ++i )
	{
		const string stretch = state.substr( i, note_size );
		cout << stretch << endl;
		if( notes.find( stretch ) != notes.end() )
		{
			cout << "=> " << notes[ stretch ] << endl;
		}
	}
	
	return 0;
}

