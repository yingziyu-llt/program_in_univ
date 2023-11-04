/*
 * Copyright (C) 2023 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future
 */

#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
using namespace std;

double f[10000];

double fib(int n)
{
	if(f[n] != 0) return f[n];
	return f[n] = f[n - 1] + f[n - 2];
}

int main()
{
#ifdef llt
	freopen("13.in","r",stdin);
	freopen("13.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	f[0] = f[1] = 1;
	double ans = 0;
	for(int i = 1;i <= n;i++)
		ans += (double)fib(i + 1) / (double)fib(i);
	printf("%.4lf",ans);
}

