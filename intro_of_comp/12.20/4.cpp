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
bool vis[100][100];
int dx[4] = {0,-1,0,1},dy[4] = {-1,0,1,0};
int n,m;
int dfs(int x,int y)
{
	int size = 1;
	for(int i = 0;i < 4;i++)
		if((map[x][y] & (1 << i)) == 0)
		{
			int xx = x + dx[i],yy = y + dy[i];
			if(xx > 0 && xx <= n && yy > 0 && yy <= m && !vis[xx][yy])
			{
				vis[xx][yy] = 1;
				size += dfs(xx,yy);
			}
		}
	return size;
}

int main()
{
#ifdef llt
	freopen("4.in","r",stdin);
	freopen("4.out","w",stdout);
#endif
	scanf("%d%d",&n,&m);
	for(int i = 1;i <= n;i++)
		for(int j = 1;j <= m;j++)
			scanf("%d",&map[i][j]);
	int maxn = 0,cnt = 0;
	for(int i = 1;i <= n;i++)
		for(int j = 1;j <= m;j++)
			if(!vis[i][j])
				{
					vis[i][j] = 1;
					int t = dfs(i,j);
					maxn = max(maxn,t);
					cnt++;
				}
	printf("%d\n%d",cnt,maxn);
}

