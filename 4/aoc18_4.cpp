#include <cstddef>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cassert>
#include <sstream>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <array>
#include <numeric>
using namespace std;

enum class GuardState { begin, sleep, wake };

ostream& operator<<( ostream& os, const GuardState& s )
{
	switch( s )
	{
		case GuardState::begin: os << "begins shift"; break;
		case GuardState::sleep: os << "falls asleep"; break;
		case GuardState::wake: os << "wakes up"; break;
		default: assert( false ); break;
	}
	return os;
}

struct Record
{
	size_t month, day, hour, minute, guard;
	GuardState state;
	string event;

	auto minutes_elapsed() const
	{
		return month * ( 31 * 24 * 60 ) + day * ( 24 * 60 ) + hour * 60 + minute;
	}
};

ostream& operator<<( ostream& os, const Record& r )
{
	os << "(1518-" << r.month << "-" << r.day << " " << r.hour << ":" << r.minute << " - guard " << r.guard << " " << r.state <<  ")";
	return os;
}

bool cmp_record( const std::unique_ptr<Record> &a, const std::unique_ptr< Record > &b )
{
	return a->minutes_elapsed() < b->minutes_elapsed();
}

int main( int argc, char* argv[] )
{
	if( argc < 2 )
	{
		cout << "Usage: " << argv[ 0 ] << " <input file>" << endl;
		return -1;
	}

	vector< unique_ptr< Record > > records;

	ifstream file( argv[ 1 ] );
	if( file.is_open() )
	{
		string line;
		while( getline( file, line ) )
		{
			assert( !line.empty() );
			istringstream iss( line );

			string token;
			iss >> token; // "[1518-06-13"
			assert( token[ 0 ] == '[' );
			assert( token[ 5 ] == '-' );
			assert( token[ 8 ] == '-' );

			auto rec = make_unique< Record >();
			rec->month = stoul( token.substr( 6, 8 ) );
			rec->day = stoul( token.substr( 9, 11 ) );
			assert( rec->month >= 1 && rec->month <= 12 );
			assert( rec->day >= 1 && rec->day <= 31 );

			iss >> token; // "00:42]"
			assert( token[ 2 ] == ':' );
			assert( token.back() == ']' );

			rec->hour = stoul( token.substr( 0, 2 ) );
			rec->minute = stoul( token.substr( 3, 5 ) );
			assert( rec->hour >= 0 && rec->hour <= 23 );
			assert( rec->minute >= 0 && rec->minute <= 59 );

			iss >> token;
			assert( token == "wakes" || token == "falls" || token == "Guard" );

			if( token == "wakes" )
			{
				rec->state = GuardState::wake;
			}
			else if( token == "falls" )
			{
				rec->state = GuardState::sleep;
			}
			else if( token == "Guard" )
			{
				rec->state = GuardState::begin;
				iss >> token; // "#2617"
				assert( token[ 0 ] == '#' );

				rec->guard = stoul( token.substr( 1 ) );
				assert( rec->guard > 0 );
			}

			records.emplace_back( move( rec ) );
		}
		file.close();
		cout << "records: " << records.size() << endl;

		sort( records.begin(), records.end(), cmp_record );

		using MinuteArray = array< size_t, 60 >;
		unordered_map< size_t, MinuteArray > sleep_counts;
		size_t current_guard = 0;
		size_t fell_asleep_minute = 0;
		for( const auto& rec : records )
		{
			switch( rec->state )
			{
				case GuardState::begin :
					current_guard = rec->guard;
					if( sleep_counts.find( current_guard ) == sleep_counts.end() )
					{
						sleep_counts[ current_guard ] = MinuteArray();
					}
					break;

				case GuardState::sleep :
					fell_asleep_minute = rec->minute;
					break;

				case GuardState::wake :
					{
						assert( fell_asleep_minute < rec->minute );
						auto& counts = sleep_counts[ current_guard ];
						for( auto i = fell_asleep_minute; i < rec->minute; ++i )
						{
							++counts[ i ];
						}
					}
					break;

				default: break;
			}
		}
		cout << "guard sleep counts: " << sleep_counts.size() << endl;

		/* PART 1 */
		size_t guard_most_asleep = 0;
		size_t max_sleep_minutes = 0;
		for( auto itr : sleep_counts )
		{
			const auto& current_guard = itr.first;
			const auto& current_sleep_counts = itr.second;

			const size_t minutes_asleep = accumulate( current_sleep_counts.begin(), current_sleep_counts.end(), 0 );
			if( minutes_asleep > max_sleep_minutes )
			{
				max_sleep_minutes = minutes_asleep;
				guard_most_asleep = current_guard;
			}
		}
		cout << "guard most asleep is " << guard_most_asleep << " for " << max_sleep_minutes << " minutes" << endl;

		size_t sleepiest_minute = 0;
		size_t max_sleep = 0;
		const auto& sleepiest_guard_counts = sleep_counts[ guard_most_asleep ];
		for( size_t i = 0; i < sleepiest_guard_counts.size() ; ++i )
		{
			const auto current_sleep = sleepiest_guard_counts[ i ];
			if( current_sleep > max_sleep )
			{
				max_sleep = current_sleep;
				sleepiest_minute = i;
			}
		}
		cout << "most asleep's sleepiest minute is " << sleepiest_minute << endl;
		cout << "1) result guard id * minute = " << ( guard_most_asleep * sleepiest_minute ) << endl;

		size_t x;
	}
	else
	{
		cerr << "ERROR: could not open file" << endl;
	}

	return -1;
}
