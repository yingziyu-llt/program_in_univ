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

double t[10000];
int main()
{
#ifdef llt
	freopen("5.in","r",stdin);
	freopen("5.out","w",stdout);
#endif
	int n,k;
	scanf("%d%d",&n,&k);
	for(int i = 1;i <= n;i++)
		scanf("%lf",&t[i]);
	sort(t + 1,t + n + 1);
	int maxn = n;
	while(true)
	{
		double total = 0;
		for(int i = 1;i <= maxn;i++)
			total += t[i];
		int xx = upper_bound(t + 1,t + 1 + maxn,total / k) - t - 1;
		if(t[maxn] <= total / k)
		{
			printf("%.3lf",total / k);
			return 0;
		}
		k -= maxn - xx;
		maxn = xx;
	}
}

