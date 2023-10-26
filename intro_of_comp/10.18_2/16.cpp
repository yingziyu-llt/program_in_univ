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
	freopen("16.in","r",stdin);
	freopen("16.out","w",stdout);
#endif
	int t;
	scanf("%d",&t);
	while(t--)
	{
		int n,m;
		int k;
		int ans = 0;
		scanf("%d%d",&n,&m);
		for(int i = 1;i <= m;i++)
			for(int j = 1;j <= n;j++)
			{
				scanf("%d",&k);
				if(i == 1 || j == 1 || i == m || j == n)
					ans += k;
			}
		printf("%d\n",ans);
	}
}
