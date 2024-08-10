#include <cstdio>
#include <cstring>
int a[ 1000 ], m;

int  total;
bool cmp( int x, int y ) {
    return x > y;
}

int  side[ 5 ];
bool dfs( int pos ) {
    if ( pos == m + 1 ) {
        bool flag = true;
        for ( int i = 1; i <= 4; i++ ) {
            if ( side[ i ] != total / 4 ) {
                flag = false;
                break;
            }
        }
        if ( flag )
            return true;
        return false;
    }
    for ( int i = 1; i <= 4; i++ ) {
        if ( side[ i ] + a[ pos ] > total / 4 ) {
            continue;
        }
        side[ i ] += a[ pos ];
        if ( dfs( pos + 1 ) )
            return true;
        side[ i ] -= a[ pos ];
    }
    return false;
}

int main() {
    int T;
    scanf( "%d", &T );
    while ( T-- ) {
        scanf( "%d", &m );
        for ( int i = 1; i <= m; i++ )
            scanf( "%d", &a[ i ] );
        memset( side, 0, sizeof( side ) );
        total = 0;

        for ( int i = 1; i <= m; i++ )
            total += a[ i ];
        if ( total % 4 != 0 ) {
            printf( "no\n" );
            continue;
        }
        if ( dfs( 1 ) )
            printf( "yes\n" );
        else
            printf( "no\n" );
    }
}
