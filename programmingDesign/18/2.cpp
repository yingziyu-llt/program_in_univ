#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <vector>

using namespace std;
int dp[ 10010 ];
int pre[ 10000 ];
int a[ 10000 ];

int check( int x, int y ) {
    if ( x - y < 0 )
        return dp[ x ];
    return dp[ x ] - check( x - y, y );
}
int main() {
    int n, x;
    memset( dp, 0, sizeof( dp ) );
    dp[ 0 ] = 1;
    scanf( "%d%d", &n, &x );
    for ( int i = 1; i <= n; i++ ) {
        int t;
        scanf( "%d", &t );
        for ( int j = x; j >= t; j-- )
            dp[ j ] += dp[ j - t ];
        a[ i ] = t;
    }
    vector< int > ans;
    for ( int i = 1; i <= n; i++ )
        if ( check( x, a[ i ] ) == 0 )
            ans.push_back( a[ i ] );
    sort( ans.begin(), ans.end() );
    printf( "%d\n", ( int )ans.size() );
    for ( auto x : ans )
        printf( "%d ", x );
    if ( ans.empty() )
        puts( "" );
    return 0;
}
