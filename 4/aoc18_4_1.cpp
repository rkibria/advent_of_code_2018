#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cassert>
#include <sstream>
#include <memory>
using namespace std;

struct Record
{
	unsigned int month, day, hour, minute;
	string event;
};

ostream& operator<<( ostream& os, const Record& r )
{
	os << "(1518-" << r.month << "-" << r.day << " " << r.hour << ":" << r.minute << " '" << r.event.c_str() <<  "')";
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

			iss >> token; // "00:42]"
			assert( token[ 2 ] == ':' );
			assert( token.back() == ']' );

			rec->hour = stoul( token.substr( 0, 2 ) );
			rec->minute = stoul( token.substr( 3, 5 ) );

			cout << *rec << endl;
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
