#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <algorithm>
using namespace std;

int main( int argc, char* argv[] )
{
	if( argc < 2 )
	{
		cout << "Usage: " << argv[ 0 ] << " <input file>" << endl;
		return -1;
	}

	using StepId = char;
	using Instruction = pair< StepId, StepId >;

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

	using StepVector = vector< StepId >;
	using AdjacencyList = unordered_map< StepId, StepVector >;

	AdjacencyList incoming;

	auto add_edge = [ &incoming ]( StepId src, StepId dst ) {
			if( incoming.find( src ) == incoming.end() )
				incoming[ src ] = StepVector();
			if( incoming.find( dst ) == incoming.end() )
				incoming[ dst ] = StepVector();
			incoming[ dst ].emplace_back( src );
		};

	for( const auto& instr : instructions )
	{
		add_edge( instr.first, instr.second );
	}

	cout << "total steps: " << incoming.size() << endl;

	auto sorted_verts_wo_incoming_edges = [ &incoming ]() {
			StepVector group;
			for( auto& itr : incoming )
			{
				if( itr.second.empty() )
				{
					group.emplace_back( itr.first );
				}
			}
			sort( group.begin(), group.end() );
			return group;
		};

	auto remove_step = [ &incoming ]( StepId step ) {
			incoming.erase( step );
			for( auto& itr : incoming )
			{
				auto& v = itr.second;
				v.erase( remove( v.begin(), v.end(), step ), v.end() );
			}
		};

	StepVector order;
	while( incoming.size() )
	{
		StepVector group = sorted_verts_wo_incoming_edges();
		for( auto s : group ) {cout << s << " ";} cout << endl;

		const StepId next_step = group.front();
		remove_step( next_step );
		order.emplace_back( next_step );
	}

	cout << "1) Order of steps: ";
	for( auto s : order )
		cout << s;
	cout << endl;

	return 0;
}
