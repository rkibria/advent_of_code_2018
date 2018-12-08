#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <vector>
#include <sstream>
#include <unordered_map>
using namespace std;

int main( int argc, char* argv[] )
{
	if( argc < 2 )
	{
		cout << "Usage: " << argv[ 0 ] << " <input file>" << endl;
		return -1;
	}

	using NodeId = char;
	using Instruction = pair< NodeId, NodeId >;
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

			NodeId step_1;
			ifs >> step_1;

			for( int i = 0; i < 5; ++i )
				ifs >> token;

			NodeId step_2;
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

	using AdjacencyElements = vector< NodeId >;
	using AdjacencyList = unordered_map< NodeId, AdjacencyElements >;

	auto initialize_entries = []( AdjacencyList& lst, NodeId src, NodeId dst ) {
			if( lst.find( src ) == lst.end() )
				lst[ src ] = AdjacencyElements();
			if( lst.find( dst ) == lst.end() )
				lst[ dst ] = AdjacencyElements();
		};

	AdjacencyList incoming, outgoing;
	for( const auto& instr : instructions )
	{
		const auto src = instr.first;
		const auto dst = instr.second;

		initialize_entries( incoming, src, dst );
		initialize_entries( outgoing, src, dst );

		incoming[ dst ].emplace_back( src );
		outgoing[ src ].emplace_back( dst );
	}

	for( auto itr : incoming )
	{
		if( itr.second.empty() )
			cout << "start node " << itr.first << endl;
	}

	for( auto itr : outgoing )
	{
		if( itr.second.empty() )
			cout << "end node " << itr.first << endl;
	}

	return 0;
}
