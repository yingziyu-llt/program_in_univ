#include <algorithm>
#include <cstddef>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef int element_t;

size_t lcs(element_t* arr_1, element_t* arr_2, size_t len_1, size_t len_2) {
    // Allocate memory once for all arrays
    element_t* prv = (element_t*)malloc(sizeof(element_t) * (len_1 + 1));
    element_t* cur = (element_t*)malloc(sizeof(element_t) * (len_1 + 1));
    element_t* prv_prv = (element_t*)malloc(sizeof(element_t) * (len_1 + 1));

    // Initialize the arrays
    memset(prv, 0, sizeof(element_t) * (len_1 + 1));
    memset(cur, 0, sizeof(element_t) * (len_1 + 1));
    memset(prv_prv, 0, sizeof(element_t) * (len_1 + 1));
    
    for (int k = 2; k <= len_1 + len_2; k++) {
        int start = std::max(1, k - (int)len_2);
        int end = std::min((int)len_1, k - 1);

        // Parallelize the loop with static scheduling
        #pragma omp parallel for schedule(static)
        for (int i = start; i <= end; i++) {
            int j = k - i;
            if (arr_1[i - 1] == arr_2[j - 1]) {
                cur[i] = prv_prv[i - 1] + 1;
            } else {
                cur[i] = std::max(prv[i], prv[i - 1]);
            }
        }

        // Swap pointers outside the parallel region to avoid synchronization overhead
        element_t* temp = prv_prv;
        prv_prv = prv;
        prv = cur;
        cur = temp;
    }

    size_t result = prv_prv[len_1]; // Final result in prv_prv

    // Free memory
    free(prv);
    free(cur);
    free(prv_prv);

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
    int result = lcs( arr_1, arr_2, n1, n2 );
    end_time   = omp_get_wtime();
    printf( "result: %d\n", result );
    printf( "Time: %lf\n", end_time - start_time );
    return 0;
}
