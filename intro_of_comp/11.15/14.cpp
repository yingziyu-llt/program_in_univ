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

struct Cricle
{
	int x,y;
	int r;
}a[100010];

bool inter(Cricle a,Cricle b)
{
	double dis = (a.y - b.y) * (a.y - b.y) + (a.x - b.x) * (a.x - b.x); 
	if(dis >= (a.r + b.r) * (a.r + b.r)) return false;
	return true;
}

int main()
{
#ifdef llt
	freopen("14.in","r",stdin);
	freopen("14.out","w",stdout);
#endif
	int n;
	scanf("%d",&n);
	for(int i = 1;i <= n;i++)
		scanf("%d%d%d",&a[i].x,&a[i].y,&a[i].r);
	bool flag = false;
	for(int i = 1;i <= n;i++)
		for(int j = i + 1;j <= n;j++)
			flag = flag | inter(a[i],a[j]);
	printf("%s",flag ? "Yes" : "No");
}

