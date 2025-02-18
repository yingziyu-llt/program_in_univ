#include <cstring>
#include <iostream>

using namespace std;
char mp[ 1000 ][ 1000 ];

int main() {
    int T;
    cin >> T;
    while ( T-- ) {
        int n, a, b;
        cin >> n >> a >> b;
        for ( int i = 1; i <= n; i++ ) {
            for ( int j = 1; j <= n; j++ ) {
                cin >> mp[ i ][ j ];
            }
        }
        int res = 0;
        for ( int i = 1; i <= n; i++ ) {
            for ( int j = 1; j <= n; j++ ) {
                if ( mp[ i ][ j ] == 'B' ) {
                    if ( i - b <= 0 || j - a <= 0 || mp[ i - b ][ j - a ] == 'W' ) {
                        res = -1;
                        break;
                    }
                    if ( mp[ i - b ][ j - a ] == 'G' )
                        mp[ i - b ][ j - a ] = 'W', res++;
                }
                if ( res == -1 )
                    break;
            }
        }
        if ( res == -1 ) {
            cout << res << endl;
            continue;
        }
        for ( int i = 1; i <= n; i++ ) {
            for ( int j = 1; j <= n; j++ ) {
                if ( mp[ i ][ j ] == 'B' || mp[ i ][ j ] == 'G' ) {
                    res++;
                    if ( i + b <= n && j + a <= n && mp[ i + b ][ j + a ] == 'G' )
                        mp[ i + b ][ j + a ] = 'W';
                }
            }
        }

        cout << res << endl;
    }
}
