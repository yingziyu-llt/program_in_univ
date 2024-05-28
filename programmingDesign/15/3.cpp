
#include <stdio.h>

void fun() {
    int n, i, j, high[ 105 ], dp[ 105 ], right = 1, left = 1;
    scanf( "%d", &n );
    for ( i = 0; i < n; i++ ) {
        scanf( "%d", &high[ i ] );
        dp[ i ] = 1;
    }
    for ( i = 0; i < n; i++ ) {
        for ( j = 0; j < i; j++ ) {
            if ( high[ j ] >= high[ i ] && dp[ i ] <= dp[ j ] + 1 ) {
                dp[ i ] = dp[ j ] + 1;
            }
            if ( right < dp[ i ] ) {
                right = dp[ i ];
            }
        }
    }
    for ( i = 0; i < n; i++ ) {
        dp[ i ] = 1;
    }
    for ( i = 0; i < n; i++ ) {
        for ( j = 0; j < i; j++ ) {
            if ( high[ j ] <= high[ i ] && dp[ i ] <= dp[ j ] + 1 ) {
                dp[ i ] = dp[ j ] + 1;
            }
            if ( left < dp[ i ] ) {
                left = dp[ i ];
            }
        }
    }
    if ( left > right ) {
        printf( "%d\n", left );
    }
    else
        printf( "%d\n", right );
}

int main() {
    int n;
    scanf( "%d", &n );
    while ( n-- ) {
        fun();
    }
    return 0;
}
