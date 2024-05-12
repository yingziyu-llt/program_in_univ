#include "cmath"
#include <algorithm>
#include <cstdio>
#include <iostream>
using namespace std;

double a[ 10005 ];
int    n, f;

const double pi = acos( -1.0 );

int split( double x ) {
    int cnt = 0;
    for ( int i = 0; i < n; i++ ) {
        cnt += ( int )( a[ i ] / x );
        if ( cnt >= f + 1 )
            return true;
    }
    return false;
}
int main() {
    cin >> n >> f;
    double smax = 0;
    for ( int i = 0; i < n; i++ ) {
        scanf( "%lf", &a[ i ] );
        a[ i ] = a[ i ] * a[ i ] * pi;
        smax   = max( smax, a[ i ] );
    }
    double rb = smax, lb = 0;
    while ( rb - lb >= 1e-5 ) {
        double mid = ( rb + lb ) / 2.0;
        if ( !split( mid ) )
            rb = mid;
        else
            lb = mid;
    }
    printf( "%.3lf", lb );
}
