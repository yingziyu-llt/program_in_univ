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

int v[1000],t[1000];

int main()
{
#ifdef llt
	freopen("17.in","r",stdin);
	freopen("17.out","w",stdout);
#endif
	int n,m;
	scanf("%d%d",&n,&m);
	for(int i = 1;i <= m;i++)
		scanf("%d,%d",&v[i],&t[i]);
	int from = 1,to = 2;
	int sum = t[1],cur = v[1],maxn = 0;
	int cost = 0,f = 0;
	for(;from <= m;from++)
	{
		to = max(to,from);
		while(to <= m && sum <= n)
		{
			sum += t[to];
			cur += v[to];
			if(maxn <= cur)
			{
				if(maxn != cur)
				{
					maxn = cur,cost = sum,f = from;
				}
				else if(cost > sum)
				{
					cost = sum,f = from;
				}
			}
			to++;
		}
		sum -= t[from],cur -= v[from];
	}
	printf("%d:%d;%d",f,maxn,cost);
}

