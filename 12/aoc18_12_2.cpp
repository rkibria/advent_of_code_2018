#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <vector>
#include <sstream>
#include <array>
#include <algorithm>
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
		assert( !token.empty() );
		state.resize( token.size() );
		transform( token.begin(), token.end(), state.begin(), char_to_bool );

		while( getline( file, line ) )
		{
			if( !line.empty() )
			{
				ifs.str( line );
				ifs.clear();
				
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
	size_t padding = 200;

	state.insert( state.begin(), padding, false );
	state.insert( state.end(), padding, false );

	vector< bool > next_state( state.size() );

	auto grow = [ &state, &next_state, &notes ]() {
			size_t hash = ( state[ 0 ] << 1 ) | state[ 1 ];
			for( size_t i = 0; i < state.size(); ++i )
			{
				const auto next_bit = ( i < state.size() - 2 ) ? ( state[ i + 2 ] ? 1 : 0 ) : 0; 
				hash = ( ( hash << 1 ) | next_bit ) & 0x1f;
				next_state[ i ] = notes[ hash ];
			}
			state.swap( next_state );
		};

	auto sum_numbers = [ &state, padding ]() {
			int sum = 0;
			for( size_t i = 0; i < state.size(); ++i )
			{
				const int pos = static_cast< int >( i ) - static_cast< int >( padding );
				if( state[ i ] )
				{
					sum += pos;
				}
			}
			return sum;
		};

	for( size_t i = 0; i < 20; ++i )
	{
		grow();
	}
	
	cout << "1) sum of numbers of plant-containing pots: " << sum_numbers() << endl;

	return 0;
}

