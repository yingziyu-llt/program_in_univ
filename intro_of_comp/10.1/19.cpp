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

double a[100010];

int main()
{
#ifdef llt
	freopen("19.in","r",stdin);
	freopen("19.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	double tot = 0;
	for(int i = 1;i <= n;i++)
		scanf("%lf",&a[i]),tot += a[i];
	printf("%.4lf ",tot / (double)n);
	double tot2 = 0;
	for(int i = 1;i <= n;i++)
		tot2 += (a[i] - tot / n) * (a[i] - tot / n);
	printf("%.4lf",sqrt(tot2 / n));
}

