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


int main()
{
#ifdef llt
	freopen("9.in","r",stdin);
	freopen("9.out","w",stdout);
#endif
	int n,m,minn,maxn;
	int ans1 = 0,ans2 = 0;
	scanf("%d%d%d%d",&n,&m,&minn,&maxn);
	for(int i = 0;i < n;i++)
		for(int j = 0;j < m;j++)
		{
			int t;
			scanf("%d",&t);
			if(t >= minn && t <= maxn)
				ans1 += i,ans2 += j;
		}
	printf("%d %d",ans1,ans2);
}

