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

char map[1000][1000];
int dx[] = {0,1,1,1,0,0,-1,-1,-1};
int dy[] = {0,1,0,-1,1,-1,1,0,-1};

void dfs(int x,int y)
{
	map[x][y] = '-';
	for(int i = 1;i <= 8;i++)
		if(map[x + dx[i]][y + dy[i]] == '#')
			dfs(x + dx[i],y + dy[i]);
}

int main()
{
#ifdef llt
	freopen("12.in","r",stdin);
	freopen("12.out","w",stdout);
#endif
	int n,m;
	scanf("%d%d",&n,&m);
	for(int i = 1;i <= n;i++)
		scanf("%s",map[i] + 1);
	int ans = 0;
	for(int i = 1;i <= n;i++)
		for(int j = 1;j <= m;j++)
			if(map[i][j] == '#')
				dfs(i,j),ans++;
	printf("%d",ans);
}

