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

class Rect
{
	public:
		int maxx,maxy,minx,miny;
		Rect()
		{
			maxx = maxy = -1000000;
			minx = miny = 1000000;
		}
		void put_point(int x,int y)
		{
			maxx = max(maxx,x),maxy = max(maxy,y),minx = min(minx,x),miny = min(miny,y);
		}
}r[10000];
bool vis[10000];
int main()
{
#ifdef llt
	freopen("3.in","r",stdin);
	freopen("3.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
	{
		int id,x,y;
		scanf("%d%d%d",&id,&x,&y);
		r[id].put_point(x,y);
		vis[id] = 1;
	}
	for(int i = 1;i <= 10000;i++)
		if(vis[i])
			printf("%d %d %d %d %d\n",i,r[i].minx,r[i].miny,r[i].maxx,r[i].maxy);
}

