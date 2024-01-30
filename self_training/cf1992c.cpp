/*
 * Copyright (C) 2024 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future!
 */

#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
using namespace std;

int a[100010];
int tot[2][100010];


void update(int type,int val,int pos)
{
	tot[type][pos] = val + tot[type][pos - 1];
}

int query(int type,int pos)
{
	return tot[type][pos];
}

int main()
{
#ifdef llt
	freopen("cf1992c.in","r",stdin);
	freopen("cf1992c.out","w",stdout);
#endif
	int T;
	scanf("%d",&T);
	while(T--)
	{
		int n;
		scanf("%d",&n);
		for(int i = 1;i <= n;i++)
			scanf("%d",&a[i]);
		update(0,1,1);
		for(int i = 2;i < n;i++)
		{
			if(a[i] - a[i - 1] > a[i + 1] - a[i])
				update(0,1,i),update(1,a[i] - a[i - 1],i);
			else
				update(1,1,i),update(0,a[i + 1] - a[i],i);
		}
		update(1,1,n);
		int q;
		scanf("%d",&q);
		while(q--)
		{
			int x,y;
			scanf("%d%d",&x,&y);
			int ans = 0;
			if(x > y)
				ans = query(1,x) - query(1,y);
			else
				ans = query(0,y - 1) - query(0,x - 1);
			printf("%d\n",ans);
		}
	}
}


