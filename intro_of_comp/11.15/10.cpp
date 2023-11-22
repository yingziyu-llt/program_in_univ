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
int cnt = 0;
int map[10][10];
int ans[100][10];
int cur[10];
void gen_ans(int dep)
{
	if(dep == 9)
	{
		cnt++;
		for(int i = 1;i <= 8;i++)
			ans[cnt][i] = cur[i];
		return ;
	}
	for(int i = 1;i <= 8;i++)
		if(map[dep][i] == 0)
		{
			map[dep][i] = dep;
			for(int j = 1;j <= 8;j++)
				if(map[j][i] == 0)
					map[j][i] = dep;
			for(int x = dep,y = i;x > 0 && y > 0;x--,y--)
				if(map[x][y] == 0)
					map[x][y] = dep;
			for(int x = dep,y = i;x < 9 && y < 9;x++,y++)
				if(map[x][y] == 0)
					map[x][y] = dep;
			for(int x = dep,y = i;x > 0 && y < 9;x--,y++)
				if(map[x][y] == 0)
					map[x][y] = dep;
			for(int x = dep,y = i;x < 9 && y > 0;x++,y--)
				if(map[x][y] == 0)
					map[x][y] = dep;
			cur[dep] = i;
			gen_ans(dep + 1);
			map[dep][i] = 0;
			for(int j = 1;j <= 8;j++)
				if(map[j][i] == dep)
					map[j][i] = 0;
			for(int x = dep,y = i;x > 0 && y > 0;x--,y--)
				if(map[x][y] == dep)
					map[x][y] = 0;
			for(int x = dep,y = i;x < 9 && y < 9;x++,y++)
				if(map[x][y] == dep)
					map[x][y] = 0;
			for(int x = dep,y = i;x > 0 && y < 9;x--,y++)
				if(map[x][y] == dep)
					map[x][y] = 0;
			for(int x = dep,y = i;x < 9 && y > 0;x++,y--)
				if(map[x][y] == dep)
					map[x][y] = 0;
			cur[dep] = 0;
		}
}

int main()
{
#ifdef llt
	freopen("10.in","r",stdin);
	freopen("10.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	gen_ans(1);
	for(int i = 1;i <= n;i++)
	{
		int x;
		scanf("%d",&x);
		for(int i = 1;i <= 8;i++)
			printf("%d",ans[x][i]);
		puts("");
	}
}

