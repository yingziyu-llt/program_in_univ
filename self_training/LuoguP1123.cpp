#include <cstring>
#include <iostream>

using namespace std;

int  a[ 10 ][ 10 ];
bool vis[ 10 ][ 10 ];
int  n, m, ans;

bool putable( int x, int y ) {
    if ( x < 1 || x > n || y < 1 || y > m )
        return false;
    if ( vis[ x ][ y ] )
        return false;
    for ( int dx = -1; dx <= 1; ++dx ) {
        for ( int dy = -1; dy <= 1; ++dy ) {
            int nx = x + dx, ny = y + dy;
            if ( nx < 1 || nx > n || ny < 1 || ny > m )
                continue;
            if ( vis[ nx ][ ny ] )
                return false;
        }
    }
    return true;
}

void dfs( int k, int cur ) {
    if ( k == n * m ) {
        ans = max( ans, cur );
        return;
    }
    int x = k / m + 1;
    int y = k % m + 1;

    // 不选当前格子
    dfs( k + 1, cur );

    // 选当前格子
    if ( putable( x, y ) ) {
        vis[ x ][ y ] = true;
        dfs( k + 1, cur + a[ x ][ y ] );
        vis[ x ][ y ] = false;
    }
}

int main() {
    int T;
    cin >> T;
    while ( T-- ) {
        ans = 0;
        cin >> n >> m;
        memset( a, 0, sizeof( a ) );
        for ( int i = 1; i <= n; ++i ) {
            for ( int j = 1; j <= m; ++j ) {
                cin >> a[ i ][ j ];
            }
        }
        memset( vis, false, sizeof( vis ) );
        dfs( 0, 0 );
        cout << ans << endl;
    }
    return 0;
}
