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

bool vis[1000000];
int ans[1000],cnt;

int main()
{
#ifdef llt
	freopen("17.in","r",stdin);
	freopen("17.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
	{
		int t;
		scanf("%d",&t);
		if(!vis[t])
			vis[t] = 1,ans[++cnt] = t;
	}
	for(int i = 1;i < cnt;i++)
		printf("%d,",ans[i]);
	printf("%d",ans[cnt]);
}

