/*
 * Copyright (C) 2024 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future!
 */

#include<stdio.h>

__global__ void add(int a,int b,int *c)
{
	*c = a + b;
}

int main()
{
	int c;
	int *dev_c;
	cudaMalloc((void**)&dev_c,sizeof(int));
	add<<<1,1>>>(2,7,dev_c);
	cudaMemcpy(&c,dev_c,sizeof(int),cudaMemcpyDeviceToHost);
	printf("%d",c);
	cudaFree(dev_c);
}

