#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cassert>
#include <sstream>
#include <memory>
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
	unsigned int month, day, hour, minute, guard;
	GuardState state;
	string event;
};

ostream& operator<<( ostream& os, const Record& r )
{
	os << "(1518-" << r.month << "-" << r.day << " " << r.hour << ":" << r.minute << " - guard " << r.guard << " " << r.state <<  ")";
	return os;
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
	}
	else
	{
		cerr << "ERROR: could not open file" << endl;
	}

	return -1;
}
