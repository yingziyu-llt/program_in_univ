#include <iostream>

using namespace std;
int a[ 50010 ];
int l, m, n;

bool chk( int x ) {
    int prv = 0;
    int cnt = 0;
    for ( int i = 1; i <= m + 1; i++ ) {
        if ( a[ i ] - prv >= x )
            prv = a[ i ];
        else
            cnt++;
        if ( cnt > n )
            return false;
    }
    return true;
}
int main() {
    cin >> l >> m >> n;
    for ( int i = 1; i <= m; i++ )
        cin >> a[ i ];
    a[ m + 1 ] = l;

    int lb = 0, rb = l;
    while ( rb - lb > 0 ) {
        int mid = ( lb + rb ) >> 1;
        if ( chk( mid ) )
            lb = mid + 1;
        else
            rb = mid;
    }
    while ( chk( lb ) )
        lb++;
    lb--;
    cout << lb << endl;
}
