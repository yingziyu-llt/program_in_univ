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

double a[10001];

int main()
{
#ifdef llt
	freopen("7.in","r",stdin);
	freopen("7.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	double maxn = 0,minn = 10000000;
	int maxc,minc;
	for(int i = 1;i <= n;i++)
	{
		scanf("%lf",&a[i]);
		if(a[i] > maxn) maxn = a[i],maxc = i;
		if(a[i] < minn) minn = a[i],minc = i;
	}
	a[minc] = -1;a[maxc] = -1;
	double tot = 0;
	maxn = 0,minn = 10000000;
	for(int i = 1;i <= n;i++)
	{
		if(a[i] != -1)
		{
			minn = min(minn,a[i]),maxn = max(maxn,a[i]);
			tot += a[i];
		}
	}
	printf("%.2lf %.2lf",tot / (n - 2),max(maxn - tot / (n - 2),tot / (n - 2) -  minn));
}

