#include <random>
#include <time.h>
#include <uchar.h>

char source[ 100000000 ];
int     hist[ 256 ] = { 0 };
int     main() {
    std::random_device                   rd;
    std::mt19937                         gen( rd() );
    std::uniform_int_distribution< int > dis( 0, 255 );
    for ( int i = 0; i < 100000000; i++ )
        source[ i ] = dis( gen );

    time_t start = clock();
#pragma omp parallel
    for ( int i = 0; i < 100000000; i++ )
#pragma omp atomic
        hist[ source[ i ] ]++;
    time_t end = clock();
    printf( "%d\n", end - start );
    return 0;
}
