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

int map[300][300];
bool vis[300][300];
int dx[5] = {0,0,1,0,-1},dy[5] = {0,1,0,-1,0};
int m,n;
void flood(int x,int y)
{
	vis[x][y] = 1;
	for(int k = 1;k <= 4;k++)
	{
		int xx = x + dx[k],yy = y + dy[k];
		if(xx > 0 && xx <= m && yy > 0 && yy <= n && map[xx][yy] <= map[x][y])
		{
			map[xx][yy] = map[x][y];
			if(!vis[xx][yy])
				flood(xx,yy);
		}
	}
}

int main()
{
#ifdef llt
	freopen("7.in","r",stdin);
	freopen("7.out","w",stdout);
#endif
	int T;
	scanf("%d",&T);
	while(T--)
	{
		scanf("%d%d",&m,&n);
		for(int i = 1;i <= m;i++)
			for(int j = 1;j <= n;j++)
				scanf("%d",&map[i][j]);
		int x,y;
		scanf("%d%d",&x,&y);
		int p;
		scanf("%d",&p);
		memset(vis,0,sizeof(vis));
		for(int i = 1;i <= p;i++)
		{
			int t1,t2;
			scanf("%d%d",&t1,&t2);
			flood(t1,t2);
		}
		if(!vis[x][y])
			puts("No");
		else
			puts("Yes");
	}
}
