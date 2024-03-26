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

char mp[110][110];
int ans = 0;
int dx[] = {0,1,1,1,0,0,-1,-1,-1},dy[] = {0,1,0,-1,1,-1,1,0,-1};

void dfs(int x,int y)
{
	ans++;
	mp[x][y] = ' ';
	for(int i = 1;i <= 8;i++)
	{
		int xx = x + dx[i],yy = y + dy[i];
		if(mp[xx][yy] == '#')
			dfs(xx,yy);
	}
}

int main()
{
#ifdef llt
	freopen("3.in","r",stdin);
	freopen("3.out","w",stdout);
#endif
	int c,r;
	scanf("%d%d",&c,&r);
	for(int i = 1;i <= r;i++)
		scanf("%s",mp[i] + 1);
	for(int i = 1;i <= r;i++)
		for(int j = 1;j <= c;j++)
			if(mp[i][j] == 'S')
			{
				dfs(i,j);
				printf("%d",ans - 1);
			}
}

