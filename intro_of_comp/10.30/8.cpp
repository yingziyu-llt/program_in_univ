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

bool vis[10000];
int main()
{
#ifdef llt
	freopen("8.in","r",stdin);
	freopen("8.out","w",stdout);
#endif
	while(1)
	{
		int n,m;
		scanf("%d%d",&n,&m);
		if(n == 0 && m == 0)
			break;
		memset(vis,0,sizeof(vis));
		int cur = 0;
		for(int i = 1;i < n;i++)
		{
			for(int cnt = 1;cnt < m;cnt++)
			{
				cur++;
				cur %= n;
				while(vis[cur])
				{
					cur = cur + 1;
					cur %= n;
				}
				cur %= n;
			}
			vis[cur] = 1;
			cur++;
			cur %= n;
			while(vis[cur])
			{
				cur = cur + 1;
				cur %= n;
			}
		}
		printf("%d\n",(cur + 1));
	}
}

