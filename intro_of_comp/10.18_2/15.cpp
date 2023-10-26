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

bool mp[110][110];
int a[110][110];
int dy[] = {1,0,-1,0},dx[] = {0,1,0,-1};

int main()
{
#ifdef llt
	freopen("15.in","r",stdin);
	freopen("15.out","w",stdout);
#endif
	int row,col;
	scanf("%d%d",&row,&col);
	for(int i = 1;i <= row;i++)
		for(int j = 1;j <= col;j++)
			mp[i][j] = 1,scanf("%d",&a[i][j]);
	int curx = 1,cury = 1;
	int xx = 0;
	for(int i = 1;i <= row * col;i++)
	{
		printf("%d\n",a[curx][cury]);
		if(mp[curx + dx[xx]][cury + dy[xx]] == 0)
			xx = (xx + 1) % 4;
		mp[curx][cury]=0;
		curx += dx[xx],cury += dy[xx];
	}
}

