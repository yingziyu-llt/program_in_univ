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

int map[100][100],ans;
int dx[5] = {0,0,1,0,-1},dy[5] = {0,1,0,-1,0};
int n,m;

void dfs(int i,int j)
{
	map[i][j] = -1;
	for(int k = 1;k <= 4;k++)
	{
		int xx = i + dx[k],yy = j + dy[k];
		if(xx >= 1 && xx <= n && yy >= 1 && yy <= m)
			if(map[xx][yy] == 1)
				dfs(xx,yy);
	}
}

int main()
{
#ifdef llt
	freopen("6.in","r",stdin);
	freopen("6.out","w",stdout);
#endif
	scanf("%d%d",&n,&m);
	for(int i = 1;i <= n;i++)
		for(int j = 1;j <= m;j++)
			scanf("%d",&map[i][j]);
	for(int i = 1;i <= n;i++)
		for(int j = 1;j <= m;j++)
		{
			if(map[i][j] == 1)
				ans++,dfs(i,j);
		}
	printf("%d",ans);
}

