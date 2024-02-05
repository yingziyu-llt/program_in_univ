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

int curcnt,cur[10];
int anscnt,ans[100][10];

void dfs(int ret,int min)
{
	if(ret == 0)
	{
		anscnt++;
		for(int i = 1;i <= curcnt;i++)
			ans[anscnt][i] = cur[i];
		return ;
	}
	for(int i = min;i <= ret / 2;i++)
	{
		cur[++curcnt] = i;
		dfs(ret - i,i);
		cur[curcnt--] = 0;
	}
	cur[++curcnt] = ret;
	dfs(0,1);
	cur[curcnt--] = 0;
}

int main()
{
#ifdef llt
	freopen("LuoguP2404.in","r",stdin);
	freopen("LuoguP2404.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	dfs(n,1);
	for(int i = 1;i < anscnt;i++)
	{
		for(int j = 1;j <= 10;j++)
		{
			if(j == 1) printf("%d",ans[i][j]);
			else if(ans[i][j] == 0) break;
			else printf("+%d",ans[i][j]);
		}
		puts("");
	}
}

