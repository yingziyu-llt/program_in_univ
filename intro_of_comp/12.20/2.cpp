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

int num[100][100];
int dy[] = {0,1,0,-1};
int dx[] = {1,0,-1,0};
bool vis[100][100];
int main()
{
#ifdef llt
	freopen("2.in","r",stdin);
	freopen("2.out","w",stdout);
#endif
	int n,m;
	scanf("%d%d",&n,&m);
	for(int i = 1;i <= n;i++)
		for(int j = 1;j <= m;j++)
			scanf("%d",&num[i][j]);
	int cur = 0;
	int curx = 1,cury = 1;
	for(int i = 0;i <= n + 1;i++) vis[i][0] = vis[i][m + 1] = 1;
	for(int i = 0;i <= m + 1;i++) vis[0][i] = vis[n + 1][i] = 1;
	printf("%d",num[1][1]),vis[1][1] = 1;
	for(int cnt = 2;cnt <= n * m;cnt++)
	{
		curx += dx[cur],cury += dy[cur];
		while(vis[curx][cury] == 1)
		{
			curx -= dx[cur],cury -= dy[cur];
			cur = (cur + 1) % 4;
			curx += dx[cur],cury += dy[cur];
		}
		vis[curx][cury] = 1;
		printf(" %d",num[curx][cury]);
	}
}

