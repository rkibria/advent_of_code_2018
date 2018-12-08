#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <vector>
#include <sstream>
#include <unordered_map>
using namespace std;

using StepId = char;
using Instruction = pair< StepId, StepId >;
using AdjacencyElements = vector< StepId >;
using AdjacencyList = unordered_map< StepId, AdjacencyElements >;

struct StepData
{
	bool visited;
	int depth;
};

using StepMap = unordered_map< StepId, StepData >;

void dfs( AdjacencyList& lst, StepId step, StepMap& smp )
{
	cout << "dfs: " << step << endl;
	smp[ step ].visited = true;

	for( const auto prev_step : lst[ step ] )
	{
		if( !smp[ prev_step ].visited )
		{
			cout << step << " -> " << prev_step << endl;
			dfs( lst, prev_step, smp );
		}
	}
}

int main( int argc, char* argv[] )
{
	if( argc < 2 )
	{
		cout << "Usage: " << argv[ 0 ] << " <input file>" << endl;
		return -1;
	}

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

			StepId step_1;
			ifs >> step_1;

			for( int i = 0; i < 5; ++i )
				ifs >> token;

			StepId step_2;
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

	auto initialize_adjacency_list = []( AdjacencyList& lst, StepId src, StepId dst ) {
			if( lst.find( src ) == lst.end() )
				lst[ src ] = AdjacencyElements();
			if( lst.find( dst ) == lst.end() )
				lst[ dst ] = AdjacencyElements();
		};

	auto initialize_step_map = []( StepMap& smp, StepId step ) {
			if( smp.find( step ) == smp.end() )
				smp[ step ] = StepData{ false, 0 };
		};

	StepMap smp;
	AdjacencyList incoming, outgoing;
	for( const auto& instr : instructions )
	{
		const auto src = instr.first;
		const auto dst = instr.second;

		initialize_step_map( smp, src );
		initialize_step_map( smp, dst );

		initialize_adjacency_list( incoming, src, dst );
		initialize_adjacency_list( outgoing, src, dst );
		incoming[ dst ].emplace_back( src );
		outgoing[ src ].emplace_back( dst );
	}

	StepId final_step{ 0 };
	for( auto itr : outgoing )
	{
		if( itr.second.empty() )
		{
			final_step = itr.first;
			cout << "final step is " << final_step << endl;
			break;
		}
	}
	assert( final_step != 0 );

	dfs( incoming, final_step, smp );

	return 0;
}
