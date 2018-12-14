#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <vector>
#include <sstream>
#include <algorithm>
#include <memory>
#include <limits>
#include <tuple>
#include <set>
using namespace std;

struct Point
{
	int x, y, vx, vy;

	void advance()
	{
		x += vx;
		y += vy;
	}
};

int main( int argc, char* argv[] )
{
	if( argc < 2 )
	{
		cout << "Usage: " << argv[ 0 ] << " <input file>" << endl;
		return -1;
	}

	vector< unique_ptr< Point > > message;

	ifstream file( argv[ 1 ] );
	if( file.is_open() )
	{
		string line, token;
		while( getline( file, line ) )
		{
			istringstream ifs( line );

			ifs >> token;
			assert( token.substr( 0, 10 ) == "position=<" );

			int x{ 0 };
			if( token.length() > 10 )
			{
				assert( token.back() == ',' );
				string x_str = token.substr( 10 );
				x_str.pop_back();
				x = stoi( x_str );
			}
			else
			{
				ifs >> token;
				assert( token.back() == ',' );
				token.pop_back();
				x = stoi( token );
			}

			int y{ 0 };
			ifs >> token;
			assert( token.length() > 0 );
			assert( token.back() == '>' );
			token.pop_back();
			y = stoi( token );

			int vx{ 0 };
			ifs >> token;
			assert( token.substr( 0, 10 ) == "velocity=<" );
			if( token.length() > 10 )
			{
				assert( token.back() == ',' );
				string vx_str = token.substr( 10 );
				vx_str.pop_back();
				vx = stoi( vx_str );
			}
			else
			{
				ifs >> token;
				assert( token.back() == ',' );
				token.pop_back();
				vx = stoi( token );
			}

			int vy{ 0 };
			ifs >> token;
			assert( token.length() > 0 );
			assert( token.back() == '>' );
			token.pop_back();
			vy = stoi( token );

			message.emplace_back( move( make_unique< Point >( Point{ x, y, vx, vy } ) ) );
		}
		file.close();
	}
	else
	{
		cerr << "ERROR: could not open file" << endl;
		return -1;
	}

	cout << "message points: " << message.size() << endl;

	int t = 0;

	auto advance = [ &message, &t ]() {
			++t;
			for( auto& p : message )
				p->advance();
		};

	auto extent = [ &message ]() {
			int x1{ numeric_limits< int >::max() },
				y1{ numeric_limits< int >::max() },
				x2{ numeric_limits< int >::min() },
				y2{ numeric_limits< int >::min() };
			for( auto& p : message )
			{
				x1 = min( x1, p->x );
				y1 = min( y1, p->y );

				x2 = max( x2, p->x );
				y2 = max( y2, p->y );
			}
			return tuple< int, int, int, int >( x1, y1, x2, y2 );
		};

	int x1 = 0, y1 = 0, x2 = 0, y2 = 0, w = 0, h = 0;

	auto print = [ &message, &x1, &y1, &x2, &y2, &w, &h, &t ]() {
			cout << "t = " << t << endl;
			cout << "w: " << w << ", h: " << h << endl;
			using Pnt = pair< int, int >;
			set< Pnt > points;
			for( auto& p : message )
				points.insert( Pnt( p->x, p->y) );
			for( auto y = y1; y <= y2; ++y )
			{
				for( auto x = x1; x <= x2; ++x )
				{
					if( points.find( Pnt( x, y ) ) == points.end() )
						cout << ".";
					else
						cout << "#";
				}
				cout << endl;
			}
		};

	do
	{
		advance();
		tie( x1, y1, x2, y2 ) = extent();
		w = x2 - x1;
		h = y2 - y1;
	} while( w > 40 && h > 40 );

	do
	{
		advance();
		tie( x1, y1, x2, y2 ) = extent();
		w = x2 - x1;
		h = y2 - y1;

		print();
	} while( w < 100 );

	return 0;
}
