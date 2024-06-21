#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <string.h>
using namespace std;

const int inf = 0x3f3f3f3f;
int       n, m;
char      mp[ 102 ][ 102 ];
int       dx[ 4 ] = { 0, 0, -1, 1 };
int       dy[ 4 ] = { 1, -1, 0, 0 };
int       f[ 102 ][ 102 ][ 12 ][ 35 ];
int       sx, sy;
int       scnt;
int       ans;

map< pair< int, int >, int > snake;

struct Node {
    int x, y, k, s, step;
};
queue< Node > q;

void init() {
    memset( f, inf, sizeof( f ) );
    snake.clear();
    scnt = 0;
    ans  = inf;
    for ( int i = 1; i <= n; i++ )
        for ( int j = 1; j <= n; j++ ) {
            cin >> mp[ i ][ j ];
            if ( mp[ i ][ j ] == 'K' )
                sx = i, sy = j, mp[ i ][ j ] = '.';
            else if ( mp[ i ][ j ] == 'S' )
                snake[ make_pair( i, j ) ] = ++scnt;
        }
    f[ sx ][ sy ][ 0 ][ 0 ] = 0;
}

bool ok( int x, int y ) {
    if ( x >= 1 && x <= n && y >= 1 && y <= n && mp[ x ][ y ] != '#' )
        return true;
    return false;
}

void bfs( Node st ) {
    if ( !q.empty() )
        q.pop();
    q.push( st );
    while ( !q.empty() ) {
        Node cur = q.front();
        q.pop();
        Node next;
        for ( int i = 0; i < 4; i++ ) {
            int newx = cur.x + dx[ i ];
            int newy = cur.y + dy[ i ];
            if ( !ok( newx, newy ) )
                continue;
            next.x = newx, next.y = newy;
            if ( mp[ newx ][ newy ] == 'S' ) {
                int th = snake[ make_pair( newx, newy ) ];
                if ( cur.s & ( 1 << ( th - 1 ) ) ) {
                    next.s    = cur.s;
                    next.k    = cur.k;
                    next.step = cur.step + 1;
                }
                else {
                    next.s    = ( cur.s | ( 1 << ( th - 1 ) ) );
                    next.k    = cur.k;
                    next.step = cur.step + 2;
                }
                if ( next.step < f[ newx ][ newy ][ next.k ][ next.s ] ) {
                    f[ newx ][ newy ][ next.k ][ next.s ] = next.step;
                    q.push( next );
                }
            }
            else if ( mp[ newx ][ newy ] >= '1' && mp[ newx ][ newy ] <= '9' ) {
                int th = mp[ newx ][ newy ] - '0';
                if ( th == cur.k + 1 )
                    next.k = cur.k + 1;
                else
                    next.k = cur.k;
                next.s    = cur.s;
                next.step = cur.step + 1;
                if ( next.step < f[ newx ][ newy ][ next.k ][ next.s ] ) {
                    f[ newx ][ newy ][ next.k ][ next.s ] = next.step;
                    q.push( next );
                }
            }
            else if ( mp[ newx ][ newy ] == '.' ) {
                next.k    = cur.k;
                next.s    = cur.s;
                next.step = cur.step + 1;
                if ( next.step < f[ newx ][ newy ][ next.k ][ next.s ] ) {
                    f[ newx ][ newy ][ next.k ][ next.s ] = next.step;
                    q.push( next );
                }
            }
            else if ( mp[ newx ][ newy ] == 'T' ) {
                next.k    = cur.k;
                next.s    = cur.s;
                next.step = cur.step + 1;
                if ( next.step < f[ newx ][ newy ][ next.k ][ next.s ] )
                    f[ newx ][ newy ][ next.k ][ next.s ] = next.step;
                if ( next.k == m )
                    ans = min( f[ newx ][ newy ][ next.k ][ next.s ], ans );
                else
                    q.push( next );
            }
        }
    }
}

int main() {
    while ( cin >> n >> m && ( n || m ) ) {
        init();
        Node temp;
        temp.x = sx, temp.y = sy, temp.k = 0, temp.s = 0, temp.step = 0;
        bfs( temp );
        if ( ans == inf )
            cout << "impossible" << endl;
        else
            cout << ans << endl;
    }
    return 0;
}
