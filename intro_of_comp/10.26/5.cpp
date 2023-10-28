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

int a[101000];
bool vis[101000];
int main()
{
#ifdef llt
	freopen("5.in","r",stdin);
	freopen("5.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
		scanf("%d",&a[i]);
	int cnt = 0;
	for(int i = 1;i <= n;i++)
	{
		if(vis[i]) continue;
		cnt++;
		int cur = i;
		while(!vis[cur])
		{
			vis[cur] = 1;
			cur = a[cur];
		}
	}
	printf("%d",cnt);
}

