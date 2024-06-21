a#include <algorithm>
#include <iostream>
#include <string>

using namespace std;

int dp[ 12980 ];

int main() {
    int n, m;
    cin >> n >> m;
    for ( int i = 1; i <= n; i++ ) {
        int w, d;
        cin >> w >> d;
        for ( int i = m; i >= w; i-- )
            dp[ i ] = max( dp[ i ], dp[ i - w ] + d );
    }
    int ans = 0;
    for ( int i = 1; i <= m; i++ )
        ans = max( ans, dp[ i ] );
    cout << ans << endl;
    return 0;
}
