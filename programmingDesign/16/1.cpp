#include <cstring>
#include <iostream>
using namespace std;

int mp[ 7 ][ 7 ];

int  dx[] = { 0, 0, 1, -1 };
int  dy[] = { 1, -1, 0, 0 };
bool dfs( int x, int y ) {
    if ( x == 1 && y == 1 ) {
        cout << "(0, 0)" << endl;
        return true;
    }
    for ( int i = 0; i <= 3; i++ ) {
        int xx = x + dx[ i ], yy = y + dy[ i ];
        if ( mp[ xx ][ yy ] == 0 ) {
            mp[ xx ][ yy ] = 1;
            if ( dfs( xx, yy ) ) {
                cout << "(" << x - 1 << ", " << y - 1 << ")" << endl;
                return true;
            }
            mp[ xx ][ yy ] = 0;
        }
    }
    return false;
}

int main() {
    memset( mp, -1, sizeof( mp ) );
    for ( int i = 1; i <= 5; i++ )
        for ( int j = 1; j <= 5; j++ )
            cin >> mp[ i ][ j ];
    dfs( 5, 5 );
}
