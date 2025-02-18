#include <iostream>

using namespace std;

int  a[ 1000010 ], sum[ 1000010 ], cnt[ 1000010 ];
bool used[ 1000010 ];

int main() {
    int n;
    cin >> n;
    long long res = 0;
    for ( int i = 1; i <= n; i++ )
        cin >> a[ i ];
    for ( int i = 1; i <= n; i++ ) {
        if ( !used[ a[ i ] ] ) {
            sum[ i ]++;
        }
        used[ a[ i ] ] = true;
        sum[ i ] += sum[ i - 1 ];
    }
    for ( int i = n; i >= 1; i-- ) {
        cnt[ a[ i ] ]++;
        if ( cnt[ a[ i ] ] == 2 ) {
            res += sum[ i - 1 ];
        }
    }
    for ( int i = 1; i <= n; i++ )
        if ( cnt[ i ] > 2 )
            res--;

    cout << res << endl;
    return 0;
}
