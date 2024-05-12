#include <algorithm>
#include <iostream>
#include <string>
using namespace std;

void flip( int i, string& s ) {
    if ( i > 0 )
        s[ i - 1 ] = s[ i - 1 ] == '0' ? '1' : '0';
    s[ i ] = s[ i ] == '0' ? '1' : '0';
    if ( i + 1 < s.length() )
        s[ i + 1 ] = s[ i + 1 ] == '0' ? '1' : '0';
}

int main() {
    std::string input, out;
    std::cin >> input >> out;
    string x( input );
    int    cnt = 0;
    int    ans = 0x7fffffff;
    for ( int i = 1; i < input.length(); i++ ) {
        if ( input[ i - 1 ] != out[ i - 1 ] )
            flip( i, input ), cnt++;
    }
    if ( input == out )
        ans = cnt;
    flip( 0, x );
    cnt = 1;
    for ( int i = 1; i < x.length(); i++ ) {
        if ( x[ i - 1 ] != out[ i - 1 ] )
            flip( i, x ), cnt++;
    }
    if ( x == out )
        ans = min( ans, cnt );
    if ( ans == 0x7fffffff )
        std::cout << "impossible" << std::endl;
    else
        std::cout << ans << endl;
}
