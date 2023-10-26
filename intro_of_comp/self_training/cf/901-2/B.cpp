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

int a[1000],b[1000];

int main()
{
#ifdef llt
	freopen("B.in","r",stdin);
	freopen("B.out","w",stdout);
#endif
	int t;
	scanf("%d",&t);
	while(t--)
	{
		int n,m,k;
		scanf("%d%d%d",&n,&m,&k);
		for(int i = 1;i <= n;i++)
			scanf("%d",&a[i]);
		for(int i = 1;i <= m;i++)
			scanf("%d",&b[i]);
		int x = 1,y = 1;
		for(int i = 1;i <= n;i++)
			if(a[x] > a[i]) x = i;
		for(int i = 1;i <= m;i++)
			if(b[y] < b[i]) y = i;
		if(a[x] < b[y]) swap(a[x],b[y]);
		k--;
		if(k & 1)
		{
			for(int i = 1;i <= n;i++)
				if(a[x] < a[i]) x = i;
			for(int i = 1;i <= m;i++)
				if(b[y] > b[i]) y = i;
			if(a[x] > b[y]) swap(a[x],b[y]);
		}
		long long sum1 = 0;
		for(int i = 1;i <= n;i++)
			sum1 += a[i];
		printf("%lld\n",sum1);
	}
}

