#include <iostream>
#include <queue>

using namespace std;

struct Point {
    int x, y, cost;
};

queue< Point > q;

char mp[ 110 ][ 110 ];
int  dx[] = { 0, 0, 1, 0, -1 };
int  dy[] = { 0, 1, 0, -1, 0 };

int main() {
    int n, m;
    cin >> n >> m;
    Point s;
    for ( int i = 1; i <= n; i++ ) {
        for ( int j = 1; j <= m; j++ ) {
            cin >> mp[ i ][ j ];
            if ( mp[ i ][ j ] == 'S' ) {
                s.x    = i;
                s.y    = j;
                s.cost = 0;
            }
        }
    }
    q.push( s );

    while ( !q.empty() ) {
        Point cur = q.front();
        q.pop();
        for ( int i = 1; i <= 4; i++ ) {
            Point nxt;
            nxt.x    = cur.x + dx[ i ];
            nxt.y    = cur.y + dy[ i ];
            nxt.cost = cur.cost + 1;

            if ( nxt.x <= 0 || nxt.y <= 0 || nxt.x > n || nxt.y > m ) {
                continue;
            }
            if ( mp[ nxt.x ][ nxt.y ] == '#' ) {
                continue;
            }
            if ( mp[ nxt.x ][ nxt.y ] == 'T' ) {
                cout << nxt.cost << endl;
                return 0;
            }
            q.push( nxt );
            mp[ nxt.x ][ nxt.y ] = '#';
        }
    }
}
