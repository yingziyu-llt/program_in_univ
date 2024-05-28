#include <iostream>
#include <string>

int  mp[ 100 ][ 100 ];
bool vis[ 100 ];

int  n, k;
int  ans;
void dfs( int x, int used ) {
    if ( k == used ) {
        ans++;
        return;
    }
    if ( k > used && x > n )
        return;
    for ( int i = 1; i <= n; i++ ) {
        if ( !vis[ i ] && mp[ x ][ i ] ) {
            vis[ i ] = 1;
            dfs( x + 1, used + 1 );
            vis[ i ] = 0;
        }
    }
    dfs( x + 1, used );
}
int main() {
    while ( std::cin >> n >> k ) {
        if ( n == -1 )
            break;
        for ( int i = 1; i <= n; i++ ) {
            std::string s;
            std::cin >> s;
            for ( int j = 0; j < s.size(); j++ )
                mp[ i ][ j + 1 ] = ( s[ j ] == '#' );
        }
        ans = 0;
        dfs( 1, 0 );
        std::cout << ans << std::endl;
    }
}
