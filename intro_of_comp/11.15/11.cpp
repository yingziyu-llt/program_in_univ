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

int map[100][100];
int n;
int xx[10];
int ans = 100000000;


void solve(int dep)
{
	if(dep == n + 1)
	{
		int tmp = 0;
		int cur = 0;
		for(int i = 1;i <= n;i++)
		{
			tmp = 0;
			for(int j = 1;j <= n;j++)
				tmp += map[j][(i + xx[j]) % n + 1];
			if(tmp > cur)
				cur = tmp;
		}
		ans = min(ans,cur);
		return ;
	}
	for(int i = 0;i < n;i++)
	{
		xx[dep] = i;
		solve(dep + 1);
	}
}

int main()
{
#ifdef llt
	freopen("11.in","r",stdin);
	freopen("11.out","w",stdout);
#endif
	while(1)
	{
		scanf("%d",&n);
		if(n == -1)
			break;
		for(int i = 1;i <= n;i++)
			for(int j = 1;j <= n;j++)
				scanf("%d",&map[i][j]);
		ans = 100000000;
		solve(1);
		printf("%d\n",ans);
	}
}

