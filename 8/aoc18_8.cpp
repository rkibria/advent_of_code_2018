#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include <sstream>
using namespace std;

using InputVector = vector< size_t >;

static size_t metadata_sum = 0;

size_t sum_all_metadata( const InputVector& inputs, size_t start_index )
{
	const size_t n_children = inputs[ start_index ];
	const size_t n_metadata = inputs[ start_index + 1 ];

	size_t node_length = 2;
	node_length += n_metadata;

	size_t children_length = 0;
	for( size_t i = 0; i < n_children; ++i )
	{
		children_length += sum_all_metadata( inputs, start_index + 2 + children_length );
	}

	if( n_metadata > 0 )
	{
		for( size_t i = 0; i < n_metadata; ++i )
		{
			metadata_sum += inputs[ start_index + 2 + children_length + i ];
		}
	}

	node_length += children_length;

	return node_length;
}

int main( int argc, char * argv[] )
{
	if( argc < 2 )
	{
		cout << "Usage: " << argv[0] << " <input file>" << endl;
		return -1;
	}

	InputVector inputs;

	ifstream file( argv[1] );
	if( file.is_open() )
	{
		string line;
		assert( getline( file, line ) );

		istringstream iss( line );
		string token;
		while( !iss.eof() )
		{
			iss >> token;
			inputs.emplace_back( stoul( token) );
		}

		file.close();
	}

	cout << "input length: " << inputs.size() << endl;

	// PART 1
	const size_t inputs_read = sum_all_metadata( inputs, 0 );
	assert( inputs_read == inputs.size() );

	cout << "1) sum of metadata: " << metadata_sum << endl;

	return 0;
}
