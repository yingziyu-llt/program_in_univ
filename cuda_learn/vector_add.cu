#include<stdio.h>
const int N = 1000;

static void HandleError( cudaError_t err,
                         const char *file,
                         int line ) {
    if (err != cudaSuccess) {
        printf( "%s in %s at line %d\n", cudaGetErrorString( err ),
                file, line );
        exit( EXIT_FAILURE );
    }
}
#define HANDLE_ERROR( err ) (HandleError( err, __FILE__, __LINE__ ))


__global__ void add(int *a,int *b,int *c)
{
	int xx = blockIdx.x;
	if(xx < N)
	{
		c[xx] = a[xx] + b[xx];
	}
}

int main()
{
	int a[10000],b[10000];
	int c[10000];
	for(int i = 0;i < N;i++)
		a[i] = i;
	for(int i = 0;i < N;i++)
		b[i] = i + 1;
	int *dev_a,*dev_b,*dev_c;
	HANDLE_ERROR(cudaMalloc((void**)&dev_a,N * sizeof(int)));
	HANDLE_ERROR(cudaMalloc((void**)&dev_b,N * sizeof(int)));
	HANDLE_ERROR(cudaMalloc((void**)&dev_c,N * sizeof(int)));
	HANDLE_ERROR(cudaMemcpy(dev_a,a,N * sizeof(int),cudaMemcpyHostToDevice));
	HANDLE_ERROR(cudaMemcpy(dev_b,b,N * sizeof(int),cudaMemcpyHostToDevice));
	HANDLE_ERROR(cudaMemcpy(dev_c,c,N * sizeof(int),cudaMemcpyHostToDevice));
	add<<<N,1>>>(dev_a,dev_b,dev_c);
	HANDLE_ERROR(cudaMemcpy(c,dev_c,N * sizeof(int),cudaMemcpyDeviceToHost));
	for(int i = 0;i < N;i++)
		printf("%d\n",c[i]);
	cudaFree(dev_a);cudaFree(dev_b);cudaFree(dev_c);
}
