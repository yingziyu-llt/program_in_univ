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

int mp[1010][1010];
int ans[1010][1010];
int sze;

int dx[] = {0,0,1,0,-1};
int dy[] = {0,1,0,-1,0};

void dp1(int x,int y)
{
	sze++;
	ans[x][y] = sze;
	for(int i = 1;i <= 4;i++)
	{
		int xx = x + dx[i],yy = y + dy[i];
		if(mp[xx][yy] + mp[x][y] == 1 && ans[xx][yy] == 0)
			dp1(xx,yy);
	}
}
void dp2(int x,int y)
{
	ans[x][y] = sze;
	for(int i = 1;i <= 4;i++)
	{
		int xx = x + dx[i],yy = y + dy[i];
		if(mp[xx][yy] + mp[x][y] == 1 && ans[xx][yy] != sze)
			dp2(xx,yy);
	}
}

int main()
{
#ifdef llt
	freopen("LuoguP1141.in","r",stdin);
	freopen("LuoguP1141.out","w",stdout);
#endif
	int n,m;
	scanf("%d %d\n",&n,&m);
	memset(mp,-1,sizeof(mp));
	for(int i = 1;i <= n;i++)
	{
		char c;
		for(int j = 1;j <= n;j++)
			scanf("%c",&c),mp[i][j] = c - '0';
		getchar();
		getchar();
	}
	for(int i = 1;i <= n;i++)
		for(int j = 1;j <= n;j++)
		{
			sze = 0;
			if(ans[i][j] == 0)
				dp1(i,j),dp2(i,j);
		}
	for(int i = 1;i <= m;i++)
	{
		int a,b;
		scanf("%d%d",&a,&b);
		printf("%d\n",ans[a][b]);
	}

}

