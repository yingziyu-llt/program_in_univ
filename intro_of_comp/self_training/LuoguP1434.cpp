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

int r,c;
int mp[110][110];
int dx[] = {0,1,0,-1,0},dy[] = {0,0,1,0,-1};
int ans[110][110];
int search(int x,int y)
{
	if(ans[x][y] != 0)
		return ans[x][y];
	for(int i = 1;i <= 4;i++)
	{
		int xx = x + dx[i],yy = y + dy[i];
		if(mp[x][y] > mp[xx][yy] && xx > 0 && yy > 0 && xx <= r && yy <= c)
			ans[x][y] = max(ans[x][y],search(x + dx[i],y + dy[i]) + 1);
	}
	return ans[x][y];
}

int main()
{
#ifdef llt
	freopen("LuoguP1434.in","r",stdin);
	freopen("LuoguP1434.out","w",stdout);
#endif
	scanf("%d%d",&r,&c);
	for(int i = 1;i <= r;i++)
		for(int j = 1;j <= c;j++)
			scanf("%d",&mp[i][j]);
	int res = 0;
	for(int i = 1;i <= r;i++)
		for(int j = 1;j <= c;j++)
			res = max(res,search(i,j));
	printf("%d",res + 1);
}
