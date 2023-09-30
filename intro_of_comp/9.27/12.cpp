/*
 * Copyright (C) 2023 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future
 */

/*
 * problem:
 *		use e = 1 + 1/1! + 1/2! + ... + 1/n! to calcuate e
 * input: n
 * output: e 
 */

#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
using namespace std;


long long fact(int n)//calculate "n!"
{
	long long res = 1;
	for(int i = 1;i <= n;i++)
		res *= i;
	return res;
}

int main()
{
#ifdef llt
	freopen("12.in","r",stdin);
	freopen("12.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	double e = 0;
	for(int i = 1;i <= n;i++)
		e = e + 1.0 / fact(i);
	printf("%.10lf",e + 1.0);
}

