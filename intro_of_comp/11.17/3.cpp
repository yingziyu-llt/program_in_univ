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

bool vis[400];
int ans[400],cnt;
int main()
{
#ifdef llt
	freopen("3.in","r",stdin);
	freopen("3.out","w",stdout);
#endif
	int n,p,m;
	scanf("%d%d%d",&n,&p,&m);
	int cur = p - 1;
	for(int i = 1;i <= n;i++)
	{
		for(int j = 1;j < m;j++)
		{
			while(vis[cur] != 0)
				cur = (cur + 1) % n;
			cur = (cur + 1) % n;
		}
		while(vis[cur] != 0)
			cur = (cur + 1) % n;

		vis[cur] = true;
		ans[++cnt] = cur + 1;
	}
	for(int i = 1;i < n;i++)
		printf("%d,",ans[i]);
	printf("%d",ans[n]);
}

