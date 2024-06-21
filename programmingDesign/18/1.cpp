#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

typedef long long ll;

ll dp[ 260 ][ 260 ];

int main( int argc, char* argv[] ) {
    for ( int i = 1; i <= 250; i++ )
        dp[ i ][ i ] = 1, dp[ i ][ i / 2 ] = 1 - i % 2;
    for ( int i = 1; i <= 250; i++ ) {
        for ( int j = 1; i - 2 * j >= j; j++ )
            for ( int k = j; i - 2 * j >= k; k++ )
                dp[ i ][ j ] += dp[ i - 2 * j ][ k ];
    }
    while ( 1 ) {
        int n;
        cin >> n;
        if ( n == 0 )
            break;
        ll ans = 0;
        for ( int i = 1; i <= n; i++ )
            ans += dp[ n ][ i ];
        cout << n << " " << ans << endl;
    }
    return 0;
}
