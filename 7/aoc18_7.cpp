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

	auto create_incoming_adjacency_list = [ & ]() {
			incoming.clear();
			for( const auto& instr : instructions )
			{
				add_edge( instr.first, instr.second );
			}
		};

	create_incoming_adjacency_list();
	cout << "total steps: " << incoming.size() << endl;

	// PART 1
	auto sorted_steps_wo_prereqs = [ &incoming ]() {
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

	StepVector order_1;
	while( incoming.size() )
	{
		StepVector ready_steps = sorted_steps_wo_prereqs();
		const StepId next_step = ready_steps.front();
		remove_step( next_step );

		order_1.emplace_back( next_step );
	}

	cout << "1) Order of steps: ";
	for( auto s : order_1 )
		cout << s;
	cout << endl;

	// PART 2
	create_incoming_adjacency_list();

	const size_t num_workers = 5;	// 2 for test
	const int base_step_time = 60;	// 0 for test

	auto step_completion_time = [ base_step_time ]( StepId step ) {
			return base_step_time + static_cast< int >( step - 'A' ) + 1;
		};

	const StepId no_task = 0;

	vector< StepId > worker_tasks( num_workers, no_task );
	unordered_map< StepId, int > task_progress;

	using IndexVector = vector< size_t >;

	auto get_free_workers = [ & ]() {
			IndexVector result;
			for( size_t i = 0; i < worker_tasks.size(); ++i )
			{
				const size_t check_index = worker_tasks.size() - 1 - i;
				if( worker_tasks[ check_index ] == no_task )
				{
					result.emplace_back( check_index );
				}
			}
			return result;
		};

	auto is_step_in_progress = [ & ]( StepId step ) {
			return task_progress.find( step ) != task_progress.end();
		};

	auto process_tasks = [ & ]() {
			for( size_t i = 0; i < worker_tasks.size(); ++i )
			{
				const auto step = worker_tasks[ i ];
				if( step != no_task )
				{
					--task_progress[ step ];
					if( task_progress[ step ] == 0 )
					{
						worker_tasks[ i ] = no_task;
						task_progress.erase( step );
						remove_step( step );
					}
				}
			}
		};

	int time = 0;
	while( true )
	{
		process_tasks();

		StepVector ready_steps = sorted_steps_wo_prereqs();
		if( ready_steps.empty() )
			break;

		IndexVector free_workers = get_free_workers();

		for( auto step : ready_steps )
		{
			if( free_workers.empty() )
				break;

			if( !is_step_in_progress( step ) )
			{
				size_t scheduled_worker = free_workers.back();
				free_workers.pop_back();

				worker_tasks[ scheduled_worker ] = step;
				task_progress[ step ] = step_completion_time( step );
			}
		}

		++time;
	}

	cout << "2) Completion time with " << num_workers << " workers and base step time " << base_step_time << ": " << time;

	return 0;
}
