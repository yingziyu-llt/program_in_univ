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

bool vis[110];

int main()
{
#ifdef llt
	freopen("1.in","r",stdin);
	freopen("1.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	int t;
	for(int i = 1;i <= n;i++)
		scanf("%d",&t),vis[t] = 1;
	for(int i = 1;i <= n;i++)
		if(!vis[i])
			printf("%d\n",i);
}

