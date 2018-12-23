#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <vector>
#include <sstream>
#include <array>
using namespace std;

int main( int argc, char* argv[] )
{
	if( argc < 2 )
	{
		cout << "Usage: " << argv[ 0 ] << " <input file>" << endl;
		return -1;
	}

	const size_t note_size = 5;
	const size_t note_count = 1 << note_size;

	vector< bool > state;
	array< bool, note_count > notes;

	auto char_to_bool = []( char c ) { return c == '#'; };

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
		for( auto c : token )
			state.emplace_back( char_to_bool( c ) );

		while( getline( file, line ) )
		{
			if( !line.empty() )
			{
				istringstream ifs( line );
				
				ifs >> token;
				assert( token.size() == note_size );
				
				size_t note_hash = 0;
				for( auto c : token )
				{
					note_hash = ( note_hash << 1 ) | ( char_to_bool( c ) ? 1 : 0 );
				}


				ifs >> token;
				assert( token == "=>" );

				ifs >> token;
				assert( token.length() == 1 );
				
				notes[ note_hash ] = char_to_bool( token[ 0 ] );
			}
		}
		file.close();
	}
	else
	{
		cerr << "ERROR: could not open file" << endl;
		return -1;
	}

	// PART 1
	/*
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
	*/

	return 0;
}

