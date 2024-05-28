#include <algorithm>
#include <cstring>
#include <iostream>
using namespace std;
int    m;
string f[ 55 ], s;
string mins( string a, string b ) {
    if ( a == "INF" )
        return b;
    if ( b == "INF" )
        return a;
    int i = 0;
    while ( a[ i ] == '0' )
        i++;
    a = a.substr( i );
    i = 0;
    while ( b[ i ] == '0' )
        i++;
    b        = b.substr( i );
    int lena = a.size(), lenb = b.size();
    if ( lena > lenb ) {
        return b;
    }
    else if ( lena < lenb ) {
        return a;
    }
    for ( int i = 0; i <= lena; i++ ) {
        if ( a[ i ] == b[ i ] )
            continue;
        if ( a[ i ] - '0' > b[ i ] - '0' ) {
            return b;
        }
        return a;
    }
    return a;
}

string adds( string a, string b ) {
    if ( a == "INF" || b == "INF" )
        return "INF";
    string s  = "";
    int    jw = 0, i = a.size() - 1, j = b.size() - 1, k = 0;
    while ( i >= 0 || j >= 0 || jw > 0 ) {
        int numa = ( i >= 0 ) ? a[ i-- ] - '0' : 0;
        int numb = ( j >= 0 ) ? b[ j-- ] - '0' : 0;
        int temp = numa + numb + jw;
        s += ( temp ) % 10 + '0';
        jw = temp / 10;
    }
    reverse( s.begin(), s.end() );
    return s;
}

int main() {
    while ( cin >> m ) {
        cin >> s;
        int n = s.size();
        for ( int i = 1; i <= n; i++ )
            f[ i ] = "INF";
        f[ 0 ] = "0";
        m++;
        for ( int i = 1; i <= m; i++ ) {
            for ( int j = n - m + i; j >= i; j-- ) {
                for ( int k = i - 1; k <= j - 1; k++ ) {
                    f[ j ] = mins( f[ j ], adds( f[ k ], s.substr( k, j - k ) ) );
                }
            }
        }
        cout << f[ n ] << endl;
    }
}
