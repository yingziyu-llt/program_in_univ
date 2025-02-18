#include <iostream>
#include <queue>

using namespace std;

char mp[ 300 ][ 300 ];

int dx[] = { 0, 0, 1, 0, -1 };
int dy[] = { 0, 1, 0, -1, 0 };

struct Point {
    int x, y, c;
};

queue< Point > q;

int main() {
    int T;
    cin >> T;
    while ( T-- ) {
        int n, m;
        cin >> n >> m;
        Point s;
        for ( int i = 1; i <= n; i++ ) {
            for ( int j = 1; j <= m; j++ ) {
                cin >> mp[ i ][ j ];
                if ( mp[ i ][ j ] == 'S' ) {
                    s.x = i;
                    s.y = j;
                    s.c = 0;
                }
            }
        }
        q.push( s );
        bool flag = false;

        while ( !q.empty() ) {
            Point cur = q.front();
            q.pop();
            for ( int i = 1; i <= 4; i++ ) {
                Point nxt;
                nxt.x = cur.x + dx[ i ];
                nxt.y = cur.y + dy[ i ];
                nxt.c = cur.c + 1;
                if ( nxt.x <= 0 || nxt.y <= 0 || nxt.x > n || nxt.y > m ) {
                    continue;
                }
                if ( mp[ nxt.x ][ nxt.y ] == '#' ) {
                    continue;
                }
                if ( mp[ nxt.x ][ nxt.y ] == 'E' ) {
                    flag = true;
                    cout << nxt.c << endl;
                    break;
                }
                q.push( nxt );
                mp[ nxt.x ][ nxt.y ] = '#';
            }
            if ( flag )
                break;
        }
        if ( !flag ) {
            cout << "oop!" << endl;
        }
        while ( !q.empty() )
            q.pop();
    }
}
