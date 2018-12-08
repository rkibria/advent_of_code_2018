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

	using AreaRow = vector< int >;
	vector< AreaRow > area;
	for( size_t i = 0; i < area_height + 2; ++i )
		area.emplace_back( AreaRow( area_width + 2, -1 ) );

	using DistanceCoordPair = pair< size_t, int >;
	vector< DistanceCoordPair > distances( coords.size() );
	for( size_t i = 0; i < area.size(); ++i )
	{
		const int y = static_cast< int >( i ) + low_bnd.y - 1;
		auto& row = area[ i ];
		for( size_t j = 0; j < row.size(); ++j )
		{
			const int x = static_cast< int >( j ) + low_bnd.x - 1;
			const Point p{ x, y };

			for( size_t k = 0; k < coords.size(); ++k )
			{
				distances[ k ] = DistanceCoordPair( manhattan( p, coords[ k ] ), k );
			}

			sort( distances.begin(), distances.end() );

			if( distances[ 0 ].first != distances[ 1 ].first )
			{
				row[ j ] = distances[ 0 ].second;
			}
		}
	}

	vector< int > area_counts( coords.size() );
	const int InfiniteCount = -1;

	for( size_t y = 0; y < area.size(); ++y )
	{
		const auto& row = area[ y ];
		const int left_nearest = row.front();
		if( left_nearest >= 0 )
			area_counts[ left_nearest ] = InfiniteCount;
		const int right_nearest = row.back();
		if( right_nearest >= 0 )
			area_counts[ right_nearest ] = InfiniteCount;
	}

	for( size_t x = 1; x < area[ 0 ].size() - 1; ++x )
	{
		const int top_nearest = area.front()[ x ];
		if( top_nearest >= 0 )
			area_counts[ top_nearest ] = InfiniteCount;
		const int bottom_nearest = area.back()[ x ];
		if( bottom_nearest >= 0 )
			area_counts[ bottom_nearest ] = InfiniteCount;
	}

	for( const auto& row : area )
		for( const auto nearest : row )
			if( nearest > 0 && area_counts[ nearest ] != InfiniteCount )
				++area_counts[ nearest ];

	const auto highest_area = max_element( area_counts.begin(), area_counts.end() );
	cout << "1) highest non infinite area is " << *highest_area << endl;

	return 0;
}
