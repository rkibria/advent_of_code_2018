#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <limits>
#include <algorithm>
using namespace std;

struct Point
{
	int x, y;
};

int manhattan( const Point& a, const Point& b )
{
	return abs( a.x - b.x ) + abs( a.y - b.y );
}

int main( int argc, char* argv[] )
{
	if( argc < 2 )
	{
		cout << "Usage: " << argv[ 0 ] << " <input file>" << endl;
		return -1;
	}

	vector< Point > coords;

	ifstream file( argv[ 1 ] );
	Point low_bnd{ numeric_limits< int >::max(), numeric_limits< int >::max() },
		up_bnd{ numeric_limits< int >::min(), numeric_limits< int >::min() };
	if( file.is_open() )
	{
		string line, token;
		while( getline( file, line ) )
		{
			istringstream ifs( line );
			ifs >> token;
			assert( token.back() == ',' );

			token.pop_back();
			const int x = stoi( token );
			ifs >> token;
			const int y = stoi( token );
			coords.emplace_back( Point{ x, y } );

			low_bnd.x = min( x, low_bnd.x );
			low_bnd.y = min( y, low_bnd.y );

			up_bnd.x = max( x, up_bnd.x );
			up_bnd.y = max( y, up_bnd.y );
		}
		file.close();
	}
	else
	{
		cerr << "ERROR: could not open file" << endl;
		return -1;
	}

	cout << "coordinates: " << coords.size() << endl;

	const auto area_width = static_cast< size_t >( up_bnd.x - low_bnd.x ) + 1;
	const auto area_height = static_cast< size_t >( up_bnd.y - low_bnd.y ) + 1;
	cout << "area bounds: " << low_bnd.x << "," << low_bnd.y << " -> " << up_bnd.x << "," << up_bnd.y
		<< " (" << area_width << " X " << area_height << ")" << endl;

	using AreaRow = vector< int >;
	vector< AreaRow > area;
	for( size_t i = 0; i < area_height + 2; ++i )
		area.emplace_back( AreaRow( area_width + 2, -1 ) );

	for( size_t i = 0; i < area.size(); ++i )
	{
		const int y = static_cast< int >( i ) + low_bnd.y - 1;
		auto& row = area[ i ];
		for( size_t j = 0; j < row.size(); ++j )
		{
			const int x = static_cast< int >( j ) + low_bnd.x - 1;
			const Point p{ x, y };
			int min_dist = numeric_limits< int >::max();
			int min_coord = -1;
			for( size_t k = 0; k < coords.size(); ++k )
			{
				const auto cur_coord = static_cast< int >( k );
				const auto dist = manhattan( p, coords[ k ] );
				if( dist == 0 )
				{
					min_coord = cur_coord;
					break;
				}
				/*
				if( dist == min_dist )
				{
					min_coord = -1;
					break;
				}
				if( dist < min_dist )
				{
					min_dist = dist;
					min_coord = cur_coord;
				}
				min_dist = min( dist, min_dist );
				*/
			}
			row[ j ] = min_coord;
		}
	}

	for( size_t i = 0; i < area.size(); ++i )
	{
		const int y = static_cast< int >( i ) + low_bnd.y - 1;
		cout << y << "\t";
		const auto& row = area[ i ];
		for( size_t j = 0; j < row.size(); ++j )
		{
			cout << row[ j ] << " ";
		}
		cout << endl;
	}

	return 0;
}
