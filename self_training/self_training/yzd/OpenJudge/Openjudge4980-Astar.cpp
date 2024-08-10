#include <cmath>
#include <cstdio>
#include <cstring>
#include <queue>
#pragma GCC optimize( "O3" )

char mp[ 210 ][ 210 ];

struct Point {
    int  x, y;
    int  time, except;
    bool operator<( const Point& b ) const {
        return this->except > b.except;
    }
    bool operator>( const Point& b ) const {
        return b > *this;
    }
    Point( int x, int y, int time ) {
        this->x    = x;
        this->y    = y;
        this->time = time;
    }
    bool operator==( const Point& b ) const {
        return this->x == b.x && this->y == b.y;
    }
};
int dx[] = { 0, 0, 1, -1 }, dy[] = { 1, -1, 0, 0 };
int dis[ 210 ][ 210 ];
int g( Point cur, Point tar ) {
    return abs( cur.x - tar.x ) + abs( cur.y - tar.y );
}
int main() {
    int T;
    scanf( "%d", &T );
    while ( T-- ) {
        int n, m;
        scanf( "%d%d", &n, &m );
        Point s( 0, 0, 0 ), t( 0, 0, 0 );
        for ( int i = 1; i <= n; i++ )
            scanf( "%s", mp[ i ] + 1 );
        for ( int i = 1; i <= n; i++ ) {
            for ( int j = 1; j <= m; j++ ) {
                if ( mp[ i ][ j ] == 'r' ) {
                    s            = Point( i, j, 0 );
                    mp[ i ][ j ] = '@';
                }
                else if ( mp[ i ][ j ] == 'a' ) {
                    t            = Point( i, j, 0 );
                    mp[ i ][ j ] = '@';
                }
            }
        }
        std::priority_queue< Point > q;
        memset( dis, 0x3f, sizeof( dis ) );

        bool flag = false;
        q.push( s );
        while ( !q.empty() ) {
            Point cur = q.top();
            q.pop();
            if ( cur == t ) {
                printf( "%d\n", cur.time );
                flag = true;
                break;
            }
            for ( int i = 0; i < 4; i++ ) {
                Point nxt = cur;
                nxt.x += dx[ i ], nxt.y += dy[ i ];
                if ( mp[ nxt.x ][ nxt.y ] == '#' )
                    continue;
                if ( nxt.x < 1 || nxt.x > n || nxt.y < 1 || nxt.y > m )
                    continue;
                if ( mp[ nxt.x ][ nxt.y ] == '@' )
                    nxt.time++;
                if ( mp[ nxt.x ][ nxt.y ] == 'x' )
                    nxt.time += 2;
                nxt.except = g( nxt, t ) + nxt.time;
                if ( dis[ nxt.x ][ nxt.y ] > nxt.time )
                    q.push( nxt ), dis[ nxt.x ][ nxt.y ] = nxt.time;
            }
        }
        if ( !flag ) {
            printf( "Impossible\n" );
        }
    }
}
