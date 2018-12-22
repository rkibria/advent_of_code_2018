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
	auto grow = [ &state, &notes, note_size ]() {	
			string next_state = state;
			for( size_t i = 0; i < state.length() - note_size; ++i )
			{
				const string stretch = state.substr( i, note_size );
				if( notes.find( stretch ) != notes.end() )
				{
					next_state[ i + ( note_size / 2 ) ] = notes[ stretch ];
				}
			}
			return next_state;
		};
	
	for( int i = 0; i < 20; ++i )
	{
		string next_state = grow();
		state = next_state;
	}
	
	int sum_1 = 0;
	for( size_t i = 0; i < state.length(); ++i )
	{
		const int pos = static_cast< int >( i ) - static_cast< int >( padding_size );
		if( state[ i ] == '#' )
		{
			sum_1 += pos;
		}
	}
	cout << "1) sum of numbers of plant-containing pots: " << sum_1 << endl;

	return 0;
}

