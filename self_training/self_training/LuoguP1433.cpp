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

class Point
{
	public:
		double x,y;
}points[10000];
double dist(Point A,Point B)
{
	return sqrt((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y));
}
int n;
double dp[1 << 16][16];
bool vis[16];
double ans = 1e13;
void dfs(int pos,int situ,int dep,double dis)
{
	if(dep == n + 1)
	{
		ans = min(ans,dis);
		return ;
	}
	for(int i = 1;i <= n;i++)
	{
		if(!vis[i])
		{
			if(dp[situ | (1 << i)][i] != 0 && dp[situ | (1 << i)][i] <= dis + dist(points[pos],points[i]))
				continue;
			dp[situ | (1 << i)][i] = dis + dist(points[pos],points[i]);
			vis[i] = 1;
			dfs(i,situ | (1 << i),dep + 1,dis + dist(points[pos],points[i]));
			vis[i] = 0;
		}
	}
}

int main()
{
#ifdef llt
	freopen("LuoguP1433.in","r",stdin);
	freopen("LuoguP1433.out","w",stdout);
#endif
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
		scanf("%lf %lf",&points[i].x,&points[i].y);
	dfs(0,0,1,0);
	printf("%.2lf",ans);
}

