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
#include<set>
using namespace std;

bool col[10][10],row[10][10],blk[100][10];
int mp[10][10];

int get_blk(int x,int y)
{
	return ((x - 1) / 3) * 3 + (y - 1) / 3;
}

void upd(int x,int y,int xx)
{
	col[x][xx] = 1;
	row[y][xx] = 1;
	blk[get_blk(x,y)][xx] = 1;
}
void rev(int x,int y,int xx)
{
	col[x][xx] = 0;
	row[y][xx] = 0;
	blk[get_blk(x,y)][xx] = 0;
}

bool ok(int x,int y,int xx)
{
	return col[x][xx] == 0 && row[y][xx] == 0 && blk[get_blk(x,y)][xx] == 0;
}

bool succ;
void dfs(int x,int y)
{
	if(y == 10)
	{
		if(x == 9)
		{
			succ = 1;
			return ;
		}
		dfs(x + 1,1);
		return ;
	}
	if(mp[x][y] != 0)
	{
		dfs(x,y + 1);
		if(succ) return ;
	}
	else
	{
		for(int i = 1;i <= 9;i++)
		{
			if(ok(x,y,i))
			{
				mp[x][y] = i;
				upd(x,y,i);
				dfs(x,y + 1);
				if(succ) break;
				mp[x][y] = 0;
				rev(x,y,i);
			}
		}
	}
	return ;
}

int main()
{
#ifdef llt
	freopen("LuoguP1784.in","r",stdin);
	freopen("LuoguP1784.out","w",stdout);
#endif
	for(int i = 1;i <= 9;i++)
	{
		for(int j = 1;j <= 9;j++)
		{
			scanf("%d",&mp[i][j]);
			if(mp[i][j] != 0)
				upd(i,j,mp[i][j]);
		}
	}

	dfs(1,1);
	for(int i = 1;i <= 9;i++)
	{
		for(int j = 1;j <= 9;j++)
			printf("%d ",mp[i][j]);
		puts("");
	}
}

