#include <cmath>
#include <cstdio>
#include <cstring>
#include <queue>

int dx[] = { 0, 0, 1, -1 }, dy[] = { 1, -1, 0, 0 };
int mp[ 202 ][ 202 ], dis[ 202 ][ 202 ];
int t;

struct Point {
    int x, y, time;
};

int main() {
    scanf( "%d", &t );
    std::queue< Point > q;
    Point               now, nxt;

    while ( t-- ) {
        int   ans = 1e9;
        int   n, m;
        Point start, target;
        scanf( "%d%d", &n, &m );
        memset( mp, -1, sizeof( mp ) );
        char c;
        for ( int i = 1; i <= n; i++ ) {
            for ( int j = 1; j <= m; j++ ) {
                scanf( " %c", &c );
                if ( c == '@' )
                    mp[ i ][ j ] = 1;
                if ( c == 'x' )
                    mp[ i ][ j ] = 2;
                if ( c == 'r' )
                    start = { i, j, 0 };
                if ( c == 'a' )
                    target = { i, j, 0 };
                dis[ i ][ j ] = 4002149;
            }
        }
        q.push( start );
        while ( !q.empty() ) {
            now = q.front();
            q.pop();
            for ( int i = 0; i < 4; i++ ) {
                nxt = now;
                nxt.x += dx[ i ];
                nxt.y += dy[ i ];
                if ( nxt.x == target.x && nxt.y == target.y ) {
                    ans = std::min( ans, now.time );
                    break;
                }
                if ( mp[ nxt.x ][ nxt.y ] != -1 && now.time + mp[ nxt.x ][ nxt.y ] < dis[ nxt.x ][ nxt.y ] ) {
                    nxt.time              = now.time + mp[ nxt.x ][ nxt.y ];
                    dis[ nxt.x ][ nxt.y ] = nxt.time;
                    q.push( nxt );
                }
            }
        }
        if ( ans == 1e9 ) {
            printf( "Impossible\n" );
        }
        else {
            printf( "%d\n", ans + 1 );
        }
    }
}

