#include <iostream>
#include <cstring>
#include <algorithm>
#include <omp.h>

using namespace std;

typedef int element_t;

size_t lcs(element_t* arr_1, element_t* arr_2, size_t len_1, size_t len_2) {
    if (len_1 > len_2) {
        return lcs(arr_2, arr_1, len_2, len_1);
    }

    // 使用滚动数组优化空间复杂度
    element_t* cur = new element_t[len_2 + 1];
    element_t* prv = new element_t[len_2 + 1];
    int i = 0,j = 0;

    #pragma omp parallel for
    for (i = 0; i <= len_2; i++) {
        prv[i] = 0;
    }
    #pragma omp parallel for
    for (i = 0; i <= len_1; i++) {
        cur[i] = 0;
    }
    for (i = 1; i <= len_1; i++) {
        #pragma omp parallel for
        for (j = 1; j <= len_2; j++) {
            if (arr_1[i - 1] == arr_2[j - 1]) {
                cur[j] = prv[j - 1] + 1;
            } else {
                cur[j] = max(prv[j], cur[j - 1]);
            }
        }
        swap(cur, prv);
    }

    size_t result = prv[len_2];

    return result;
}

int main() {
    freopen( "input.dat", "r", stdin );
    freopen( "output.dat", "w", stdout );
    int n1, n2;
    scanf( "%d %d", &n1, &n2 );
    element_t arr_1[ n1 ], arr_2[ n2 ];
    for ( int i = 0; i < n1; i++ )
        scanf( "%d", &arr_1[ i ] );
    for ( int i = 0; i < n2; i++ )
        scanf( "%d", &arr_2[ i ] );
    double start_time, end_time;
    start_time = omp_get_wtime();
    printf( "%d\n", lcs( arr_1, arr_2, n1, n2 ) );
    end_time = omp_get_wtime();
    printf( "Time: %lf\n", end_time - start_time );
    return 0;
}